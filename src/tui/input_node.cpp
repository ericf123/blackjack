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
    EventHandler<InputGetAndMapKeyPress> getAndMapKeyHandler =
        [this](const WrappedEvent<InputGetAndMapKeyPress>& e) {
          const auto userInputChar = getch();
          if (userInputChar == this->hitKey) {
            e.router.broadcast(this->sourceNode,
                               TuiPlayerActionCmd{ PlayerAction::Hit });
          } else if (userInputChar == this->standKey) {
            e.router.broadcast(this->sourceNode,
                               TuiPlayerActionCmd{ PlayerAction::Stand });
          } else if (userInputChar == this->doubleKey) {
            e.router.broadcast(this->sourceNode,
                               TuiPlayerActionCmd{ PlayerAction::DoubleDown });
          } else if (userInputChar == this->splitKey) {
            e.router.broadcast(this->sourceNode,
                               TuiPlayerActionCmd{ PlayerAction::Split });
          }
        };

    EventHandler<InputBlockUntilKeyPressed> blockForKeyPressHandler =
        [](const WrappedEvent<InputBlockUntilKeyPressed>& e) {
          (void)e;
          getch();
        };

    r->listen(sourceNode, false, getAndMapKeyHandler);
    r->listen(sourceNode, false, blockForKeyPressHandler);
  }
}