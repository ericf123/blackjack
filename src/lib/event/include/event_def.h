#pragma once

#include <cstdint>
#include <limits>
#include <memory>

using NodeId = uint8_t;
constexpr NodeId BROADCAST_ID = std::numeric_limits<NodeId>::max();
using OwningHandle = std::shared_ptr<NodeId>;
class EventRouter;