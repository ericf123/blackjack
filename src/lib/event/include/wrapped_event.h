#include "event_def.h"
#include <memory>

// TODO: event type validation

using EventId = const void*;

template <typename Return, template <typename> typename Event>
struct EventIdPtr {
  static const Event<Return>* const id;
};
template <typename Return, template <typename> typename Event>
const Event<Return>* const EventIdPtr<Return, Event>::id = nullptr;

template <typename Return, template <typename> typename Event>
constexpr EventId getEventId() {
  return &EventIdPtr<Return, Event>::id;
}

template <typename Return, template <typename> typename Event>
struct WrappedEvent {
  static constexpr EventId eventId = getEventId<Return, Event>();
  const NodeId sourceId;
  const NodeId destId;
  EventRouter& router;
  const Event<Return>& event;
};