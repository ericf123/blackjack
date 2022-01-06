#pragma once

#include "event_def.h"
#include "wrapped_event.h"
#include <any>
#include <cassert>
#include <functional>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <unordered_set>

template <typename Return, template <typename> typename Event>
using EventHandler = std::function<Return(const WrappedEvent<Return, Event>&)>;

class EventRouter {
public:
  EventRouter() : nextId(0) {}

  OwningHandle requestId() {
    // TODO: prevent id overflow
    const auto assignedId = nextId++;
    auto handle = std::make_shared<NodeId>(assignedId);
    nodes[assignedId] = handle;
    return handle;
  }

  template <typename Return, template <typename> typename Event>
  std::optional<Return> invoke(OwningHandle source, const NodeId& dest,
                               const Event<Return>& e, bool isPrivate) {
    std::optional<Return> result;
    invokeImpl<true>(source, dest, e, isPrivate, result);
    return result;
  }

  template <template <typename> typename Event>
  void invoke(OwningHandle source, const NodeId& dest, const Event<void>& e,
              bool isPrivate) {
    std::optional<int> _fake;
    invokeImpl<false>(source, dest, e, isPrivate, _fake);
  }

  template <typename Return, template <typename> typename Event>
  std::optional<Return> invoke(OwningHandle source, const NodeId& dest,
                               const Event<Return>& e) {
    return invoke(source, dest, e, false);
  }

  template <template <typename> typename Event>
  void invoke(OwningHandle source, const NodeId& dest, const Event<void>& e) {
    return invoke(source, dest, e, false);
  }

  template <typename Return, template <typename> typename Event>
  std::vector<Return> broadcast(OwningHandle source, const Event<Return>& e) {
    std::vector<Return> results;
    broadcastImpl<true>(source, e, results);
    return results;
  }

  template <template <typename> typename Event>
  void broadcast(OwningHandle source, const Event<void>& e) {
    std::vector<int> _fake;
    broadcastImpl<false>(source, e, _fake);
  }

  template <typename Return, template <typename> typename Event>
  std::optional<Return> invokeFirstAvailable(OwningHandle source,
                                             const Event<Return>& e) {
    const auto results = broadcast(source, e);
    if (results.empty()) {
      return std::nullopt;
    }

    return { results.front() };
  };

  template <typename Return, template <typename> typename Event>
  void listen(OwningHandle dest, bool promiscuous,
              EventHandler<Return, Event> handler) {
    const auto eventId = getEventId<Return, Event>();
    const auto destId = *dest;
    if (routes.find(eventId) == routes.end()) {
      routes.emplace(std::make_pair(eventId, RouteTable{}));
    }

    routes[eventId].map[destId] = handler;

    if (promiscuous) {
      routes[eventId].promiscuousListeners.emplace(destId);
    }
  }

  void unlisten(OwningHandle dest, const EventId& eventId) {
    const auto destId = *dest;
    if (routes.find(eventId) != routes.end()) {
      routes[eventId].map.erase(destId);
      routes[eventId].promiscuousListeners.erase(destId);
    }
  }

private:
  using RouteMap = std::unordered_map<NodeId, std::any>;
  struct RouteTable {
    RouteMap map;
    std::unordered_set<NodeId> promiscuousListeners;
  };
  enum class RouteStatus {
    Valid,
    InvalidEvent,
    NodeExpired,
    HandlerNotFound,
    Loop
  };
  size_t nextId;
  std::unordered_map<NodeId, std::weak_ptr<NodeId>> nodes;
  std::unordered_map<EventId, RouteTable> routes;

  template <typename Return, template <typename> typename Event>
  WrappedEvent<Return, Event> createWrapped(const NodeId& source,
                                            const NodeId& dest,
                                            const Event<Return>& e) {
    return WrappedEvent<Return, Event>{ source, dest, *this, e };
  }

  template <bool WriteOutput, typename Return,
            template <typename> typename Event, typename OutType>
  void invokeImpl(OwningHandle source, const NodeId& dest,
                  const Event<Return>& e, bool isPrivate,
                  std::optional<OutType>& output) {
    static_assert(!WriteOutput || std::is_same<Return, OutType>());
    const auto eventId = getEventId<Return, Event>();

    if (routes.find(eventId) != routes.end()) {
      auto& routeTable = routes[eventId];
      const auto wrapped = createWrapped(*source, dest, e);

      std::unordered_set<NodeId> deadNodes;

      const auto routeStatus = validateRoute(*source, dest, eventId);
      if (routeStatus == RouteStatus::Valid) {
        if constexpr (WriteOutput) {
          output.emplace(invokeUnsafe(source, dest, wrapped));
        } else {
          invokeUnsafe(source, dest, wrapped);
        }
      } else if (routeStatus == RouteStatus::NodeExpired) {
        deadNodes.insert(dest);
      }

      if (!isPrivate) {
        for (const auto& node : routeTable.promiscuousListeners) {
          // avoid invoking the same handler twice for promiscuous handlers that
          // are invoked directly
          if (node != dest) {
            const auto routeStatus = validateRoute(*source, node, eventId);
            if (routeStatus == RouteStatus::Valid) {
              invokeUnsafe(source, node, wrapped);
            } else if (routeStatus == RouteStatus::NodeExpired) {
              deadNodes.insert(node);
            }
          }
        }
      }

      for (const auto& node : deadNodes) {
        routeTable.map.erase(node);
        routeTable.promiscuousListeners.erase(node);
        releaseId(node);
      }
    }
  }

  template <bool WriteOutput, typename Return,
            template <typename> typename Event, typename OutType>
  void broadcastImpl(OwningHandle source, const Event<Return>& e,
                     std::vector<OutType>& output) {
    static_assert(!WriteOutput || std::is_same<Return, OutType>());

    const auto eventId = getEventId<Return, Event>();
    if (routes.find(eventId) != routes.end()) {
      std::unordered_set<NodeId> deadNodes;

      const auto wrapped = createWrapped(*source, BROADCAST_ID, e);
      for (const auto& [destId, _] : routes[eventId].map) {
        const auto routeStatus = validateRoute(*source, destId, eventId);
        if (routeStatus == RouteStatus::Valid) {
          if constexpr (WriteOutput) {
            output.push_back(invokeUnsafe(source, destId, wrapped));
          } else {
            invokeUnsafe(source, destId, wrapped);
          }
        } else if (routeStatus == RouteStatus::NodeExpired) {
          deadNodes.insert(destId);
        }
      }

      for (const auto& nodeId : deadNodes) {
        routes[eventId].map.erase(nodeId);
        routes[eventId].promiscuousListeners.erase(nodeId);
        releaseId(nodeId);
      }
    }
  }

  // sends from source to dest
  // neither e.sourceId nor e.destId are used to determine routing
  // this allows a node to differentiate between receiving a packet in
  // promiscuous mode vs. receiving a packet from a direct route
  // TODO: validate R can be stored in optional?
  template <typename Return, template <typename> typename Event>
  Return invokeUnsafe(OwningHandle source, const NodeId& dest,
                      const WrappedEvent<Return, Event>& e) {
    assert(*source == e.sourceId);
    return std::any_cast<EventHandler<Return, Event>>(
        routes[e.eventId].map[dest])(e);
  }

  RouteStatus validateRoute(const NodeId& sourceId, const NodeId& destId,
                            const EventId& eventId) {
    auto retVal = RouteStatus::Valid;

    if (!validateNode(destId)) {
      retVal = RouteStatus::NodeExpired;
    } else if (routes[eventId].map.find(destId) == routes[eventId].map.end()) {
      retVal = RouteStatus::HandlerNotFound;
    } else if (sourceId == destId) {
      retVal = RouteStatus::Loop;
    }

    return retVal;
  }

  bool validateNode(const NodeId& nodeId) {
    return nodes.find(nodeId) != nodes.end() && !nodes[nodeId].expired();
  }

  void releaseId(const NodeId& id) { nodes.erase(id); }
};