#pragma once

#include "event_def.h"
#include "wrapped_event.h"
#include <any>
#include <cassert>
#include <cxxabi.h>
#include <functional>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <unordered_set>

template <typename R, typename E>
using InvokeHandler = std::function<R(const WrappedEvent<E>&)>;

template <typename E> using EventHandler = InvokeHandler<void, E>;

using RouteTable = std::unordered_map<EventId, std::any>;
// using RouteTable = std::unordered_map<EventId, std::function<void()>>;

class EventRouter {
public:
  EventRouter() : nextId(0) {}

  OwningHandle requestId() {
    const auto assignedId = nextId++;
    auto handle = std::make_shared<NodeId>(assignedId);
    nodes[assignedId] = handle;
    routes[assignedId] = {};
    return handle;
  }

  template <typename Event>
  void send(OwningHandle source, const NodeId& dest, const Event& e,
            bool isPrivate) {
    if (validateNode(dest)) {
      auto wrapped = createWrapped(*source, dest, e);
      sendImpl(source, dest, wrapped);

      const auto eventId = wrapped.eventId;
      if (!isPrivate &&
          promiscuousListeners.find(eventId) != promiscuousListeners.end()) {
        std::unordered_set<NodeId> deadNodes;
        auto& listeners = promiscuousListeners[eventId];

        for (const auto& nodeId : listeners) {
          if (validateNode(nodeId)) {
            sendImpl(source, nodeId, wrapped);
          } else {
            deadNodes.insert(nodeId);
          }
        }

        for (const auto& nodeId : deadNodes) {
          listeners.erase(nodeId);
          releaseId(nodeId);
        }
      }
    } else {
      releaseId(dest);
    }
  }

  template <typename Event>
  void send(OwningHandle source, const NodeId& dest, const Event& e) {
    send(source, dest, e, false);
  }

  // TODO: merge Invoke/Event handler processing (invoke/send)
  template <typename R, typename Event>
  std::optional<R> invoke(OwningHandle source, const NodeId& dest,
                          const Event& e) {
    if (validateNode(dest)) {
      return invokeImpl<R, Event>(source, dest,
                                  createWrapped(*source, dest, e));
    }

    releaseId(dest);
    return std::nullopt;
  }

  template <typename R, typename Event>
  std::vector<R> broadcastInvoke(OwningHandle source, const Event& e) {
    std::unordered_set<NodeId> deadNodes;
    std::vector<R> results;

    auto wrapped = createWrapped(*source, BROADCAST_ID, e);
    for (const auto& [destId, _] : nodes) {
      if (validateNode(destId)) {
        auto result = invokeImpl<R, Event>(source, destId, wrapped);
        if (result) {
          results.push_back(result.value());
        }
      } else {
        deadNodes.insert(destId);
      }
    }

    for (const auto& nodeId : deadNodes) {
      releaseId(nodeId);
    }

    return results;
  }

  template <typename R, typename Event>
  std::optional<R> invokeFirstAvailable(OwningHandle source, const Event& e) {
    const auto results = broadcastInvoke<R>(source, e);
    if (results.empty()) {
      return std::nullopt;
    }

    return { results.front() };
  };

  template <typename Event>
  void broadcast(OwningHandle source, const Event& e) {
    std::unordered_set<NodeId> deadNodes;
    auto wrapped = createWrapped(*source, BROADCAST_ID, e);
    for (const auto& [destId, _] : nodes) {
      if (validateNode(destId)) {
        sendImpl(source, destId, wrapped);
      } else {
        deadNodes.insert(destId);
      }
    }

    for (const auto& nodeId : deadNodes) {
      releaseId(nodeId);
    }
  }

  template <typename Event>
  void listen(OwningHandle dest, bool promiscuous,
              EventHandler<Event> handler) {
    listenImpl(dest, promiscuous, handler);
  }

  template <typename R, typename Event>
  void registerInvokeHandler(OwningHandle dest,
                             InvokeHandler<R, Event> handler) {
    listenImpl(dest, false, handler);
  }

  void unlisten(OwningHandle dest, const EventId& eventId) {
    const auto destId = *dest;
    if (validateNode(destId)) {
      // attempt to remove from promiscuous listeners
      if (promiscuousListeners.find(eventId) != promiscuousListeners.end()) {
        promiscuousListeners[eventId].erase(destId);
      }
      // remove from dest's route table
      routes[destId].erase(eventId);
    }
  }

private:
  size_t nextId;
  std::unordered_map<NodeId, std::weak_ptr<NodeId>> nodes;
  // TODO: merge promiscuous and routes
  // umap<EventId, umap<NodeId, any(Invoke/EventHandler)>>
  std::unordered_map<EventId, std::unordered_set<NodeId>> promiscuousListeners;
  std::unordered_map<NodeId, RouteTable> routes;

  template <typename Event>
  WrappedEvent<Event> createWrapped(const NodeId& source, const NodeId& dest,
                                    const Event& e) {
    return WrappedEvent<Event>{ source, dest, *this, e };
  }

  // sends from source to dest
  // neither e.sourceId nor e.destId are used to determine routing
  // this allows a node to differentiate between receiving a packet in
  // promiscuous mode vs. receiving a packet from a direct route
  // TODO: validate R can be stored in optional?
  template <typename R, typename Event>
  std::optional<R> invokeImpl(OwningHandle source, const NodeId& dest,
                              const WrappedEvent<Event>& e) {
    assert(*source == e.sourceId);

    if (validateNode(dest) && *source != dest) {
      auto destRoutes = routes[dest];
      if (destRoutes.find(e.eventId) != destRoutes.end()) {
        assert(typeid(InvokeHandler<R, Event>) == destRoutes[e.eventId].type());
        return { std::any_cast<InvokeHandler<R, Event>>(destRoutes[e.eventId])(
            e) };
      }
    }

    return std::nullopt;
  }

  template <typename Event>
  void sendImpl(OwningHandle source, const NodeId& dest,
                const WrappedEvent<Event>& e) {
    assert(*source == e.sourceId);

    if (validateNode(dest) && *source != dest) {
      auto destRoutes = routes[dest];
      if (destRoutes.find(e.eventId) != destRoutes.end()) {
        std::any_cast<EventHandler<Event>>(destRoutes[e.eventId])(e);
      }
    }
  }

  template <typename R, typename Event>
  void listenImpl(OwningHandle dest, bool promiscuous,
                  InvokeHandler<R, Event> callback) {
    const auto eventId = getEventId<Event>();
    const auto destId = *dest;
    if (validateNode(destId)) {
      if (promiscuous) {
        if (promiscuousListeners.find(eventId) != promiscuousListeners.end()) {
          promiscuousListeners[eventId].insert(destId);
        } else {
          promiscuousListeners[eventId] = { destId };
        }
      }

      routes[destId][eventId] = callback;
    }
  }

  bool validateNode(const NodeId& nodeId) {
    auto retVal = false;

    if (nodes.find(nodeId) != nodes.end()) {
      if (!nodes[nodeId].expired()) {
        retVal = true;
      }
    }

    return retVal;
  }

  void releaseId(const NodeId& id) {
    nodes.erase(id);
    routes.erase(id);
  }
};