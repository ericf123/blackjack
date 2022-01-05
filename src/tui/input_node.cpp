#include "input_node.h"
#include "blackjack_events.h"
#include "event_router.h"
#include <curses.h>

InputNode::InputNode(int hitKey, int standKey, int doubleKey, int splitKey,
                     std::weak_ptr<EventRouter> router, OwningHandle sourceNode)
    : hitKey(hitKey), standKey(standKey), doubleKey(doubleKey),
      splitKey(splitKey), sourceNode(sourceNode) {
  if (!router.expired()) {
    auto r = router.lock();
    InvokeHandler<PlayerAction, InputPlayerActionInv> playerActionInvHandler =
        [this, r](const WrappedEvent<InputPlayerActionInv>& e) -> PlayerAction {
      (void)e;
      return getDesiredAction();
    };
    r->registerInvokeHandler(sourceNode, playerActionInvHandler);
  }
}

PlayerAction InputNode::getDesiredAction() {
  auto action = PlayerAction::InvalidInput;
  while (action == PlayerAction::InvalidInput) {
    auto userInputChar = getch();
    if (userInputChar == hitKey) {
      action = PlayerAction::Hit;
    } else if (userInputChar == standKey) {
      action = PlayerAction::Stand;
    } else if (userInputChar == doubleKey) {
      action = PlayerAction::DoubleDown;
    } else if (userInputChar == splitKey) {
      action = PlayerAction::Split;
    } else {
      action = PlayerAction::InvalidInput;
    }
  }

  return action;
}
