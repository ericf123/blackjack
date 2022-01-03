#include "event_def.h"
#include <memory>

// TODO: event type validation

using EventId = const void*;

template <typename T> struct EventIdPtr { static const T* const id; };
template <typename T> const T* const EventIdPtr<T>::id = nullptr;

template <typename T> constexpr EventId getEventId() {
  return &EventIdPtr<T>::id;
}

template <typename E> struct WrappedEvent {
  static constexpr EventId eventId = getEventId<E>();
  const NodeId sourceId;
  NodeId destId; // multiple destinations for broadcast
  EventRouter& router;
  const E& event;
};