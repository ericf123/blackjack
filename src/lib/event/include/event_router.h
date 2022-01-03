#pragma once

#include "wrapped_event.h"
#include <any>
#include <functional>
#include <unordered_map>
#include <unordered_set>

template <typename E>
using OnEventCallback = std::function<void(const WrappedEvent<E>&)>;
using RouteTable = std::unordered_map<EventId, std::any>;

class EventRouter {
public:
  EventRouter() : nextId(0) {}

  NodeId requestId(std::weak_ptr<void> node) {
    const auto assignedId = nextId++;
    nodes[assignedId] = node;
    routes[assignedId] = {};
    return assignedId;
  }

  void releaseId(const NodeId& id) {
    nodes.erase(id);
    routes.erase(id);
  }

  template <typename Event>
  void send(const NodeId& source, const NodeId& dest, const Event& e,
            bool isPrivate) {
    auto wrapped = createWrapped(source, dest, e);
    sendImpl(dest, wrapped);

    const auto eventId = wrapped.eventId;
    if (!isPrivate &&
        promiscuousListeners.find(eventId) != promiscuousListeners.end()) {
      std::unordered_set<NodeId> deadNodes;
      auto& listeners = promiscuousListeners[eventId];

      for (const auto& nodeId : listeners) {
        if (validateNode(nodeId)) {
          sendImpl(nodeId, wrapped);
        } else {
          deadNodes.insert(nodeId);
        }
      }

      for (const auto& nodeId : deadNodes) {
        listeners.erase(nodeId);
        releaseId(nodeId);
      }
    }
  }

  template <typename Event>
  void send(const NodeId& source, const NodeId& dest, const Event& e) {
    send(source, dest, e, false);
  }

  template <typename Event>
  void broadcast(const NodeId& source, const Event& e) {
    if (validateNode(source)) {
      std::unordered_set<NodeId> deadNodes;
      auto wrapped = createWrapped(source, 0, e);
      for (const auto& [destId, _] : nodes) {
        if (validateNode(destId)) {
          wrapped.destId = destId;
          sendImpl(destId, wrapped);
        } else {
          deadNodes.insert(destId);
        }
      }

      for (const auto& nodeId : deadNodes) {
        releaseId(nodeId);
      }
    } else {
      releaseId(source);
    }
  }

  template <typename Event>
  void listen(const NodeId& dest, OnEventCallback<Event> callback,
              bool promiscuous) {
    const auto eventId = getEventId<Event>();
    if (validateNode(dest)) {
      if (promiscuous) {
        if (promiscuousListeners.find(eventId) != promiscuousListeners.end()) {
          promiscuousListeners[eventId].insert(dest);
        } else {
          promiscuousListeners[eventId] = { dest };
        }
      }

      routes[dest][eventId] = callback;
    }
  }

  void unlisten(const NodeId& dest, const EventId& eventId) {
    if (validateNode(dest)) {
      // attempt to remove from promiscuous listeners
      if (promiscuousListeners.find(eventId) != promiscuousListeners.end()) {
        promiscuousListeners[eventId].erase(dest);
      }
      // remove from dest's route table
      routes[dest].erase(eventId);
    }
  }

private:
  size_t nextId;
  std::unordered_map<NodeId, std::weak_ptr<void>> nodes;
  std::unordered_map<EventId, std::unordered_set<NodeId>> promiscuousListeners;
  std::unordered_map<NodeId, RouteTable> routes;

  template <typename Event>
  WrappedEvent<Event> createWrapped(const NodeId& source, const NodeId& dest,
                                    const Event& e) {
    return WrappedEvent<Event>{ source, dest, *this, e };
  }

  // sends from e.sourceId to dest
  // e.destId is not used to determine the actual destination
  // this allows a node to differentiate between receiving a packet in
  // promiscuous mode vs. receiving a packet from a direct route
  template <typename Event>
  void sendImpl(const NodeId& dest, const WrappedEvent<Event>& e) {
    if (validateNode(e.sourceId) && validateNode(dest) && e.sourceId != dest) {
      auto destRoutes = routes[dest];
      if (destRoutes.find(e.eventId) != destRoutes.end()) {
        std::any_cast<OnEventCallback<Event>>(destRoutes[e.eventId])(e);
      }
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
};