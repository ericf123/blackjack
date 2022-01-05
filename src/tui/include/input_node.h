#pragma once
#include "event_def.h"
#include "player_action.h"
class InputNode {
public:
  InputNode(int hitKey, int standKey, int doubleKey, int splitKey,
            std::weak_ptr<EventRouter> router, OwningHandle sourceNode);

private:
  int hitKey;
  int standKey;
  int doubleKey;
  int splitKey;
  OwningHandle sourceNode;
};
