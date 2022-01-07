#include "dealer.h"
#include "blackjack_events.h"
#include "event_def.h"
#include "event_router.h"
#include "table.h"
#include <functional>
#include <memory>
#include <optional>

Dealer::Dealer(std::weak_ptr<EventRouter> router, OwningHandle sourceNode)
    : sourceNode(sourceNode), router(router) {
  // TODO: round end handler
  dealerHands.push_front(Hand());
  dealerHand = dealerHands.begin();

  if (auto r = router.lock()) {
    if (auto tableOpt = r->invokeFirstAvailable(
            sourceNode, ToConstRefInv<std::reference_wrapper<const Table>>{})) {
      const auto& table = tableOpt.value().get();
      endPlayer = table.getEndPlayer();
    }

    EventHandler<void, PlayerActionCmd> playerActionHandler =
        [this](const WrappedEvent<void, PlayerActionCmd>& e) {
          if (currPlayer != endPlayer && e.sourceId == *currPlayer) {
            const auto playerOpt = e.router.invoke(
                this->sourceNode, e.sourceId,
                ToConstRefInv<std::reference_wrapper<const Player>>{});

            if (playerOpt) {
              const auto& player = playerOpt.value().get();
              auto action = e.event.action;

              auto drawCard = action == PlayerAction::Hit;

              if (action == PlayerAction::Split) {
                const auto debitAmount = -1 * player.getCurrentHandWager();
                e.router.invoke(this->sourceNode, *currPlayer,
                                AdjustBankrollCmd<void>{
                                    static_cast<Bankroll>(debitAmount) });
                e.router.invoke(this->sourceNode, *currPlayer,
                                PlayerSplitHandCmd<void>{});
              }

              if (action == PlayerAction::DoubleDown) {
                const auto debitAmount = -1 * player.getCurrentHandWager();
                e.router.invoke(this->sourceNode, *currPlayer,
                                AdjustBankrollCmd<void>{
                                    static_cast<Bankroll>(debitAmount) });
                e.router.invoke(this->sourceNode, *currPlayer,
                                PlayerDoubleHandCmd<void>{});

                drawCard = true;

                if (player.playingLastHand()) {
                  action = PlayerAction::EndTurn;
                } else {
                  action = PlayerAction::Stand;
                }
              }

              // deal player a card if necessary
              if (drawCard) {
                e.router.broadcast(this->sourceNode,
                                   CardReq<void>{ *currPlayer, false });
              }

              // check if player busted
              if (player.getCurrentHandValue() > Hand::MAX_VALUE) {
                if (player.playingLastHand()) {
                  action = PlayerAction::EndTurn;
                } else {
                  action = PlayerAction::Stand;
                }
              }

              if (action == PlayerAction::Stand ||
                  action == PlayerAction::EndTurn) {
                e.router.invoke(this->sourceNode, *currPlayer,
                                PlayerEndHandCmd<void>{});
              }

              if (action == PlayerAction::EndTurn) {
                currPlayer = std::next(currPlayer);
              }
            }
          }
        };

    EventHandler<void, StartRoundCmd> startRoundHandler =
        [this](const WrappedEvent<void, StartRoundCmd>& e) {
          (void)e;
          dealerHand->reset();

          const auto tableOpt = e.router.invokeFirstAvailable(
              this->sourceNode,
              ToConstRefInv<std::reference_wrapper<const Table>>{});
          if (tableOpt) {
            const auto& table = tableOpt.value().get();
            currPlayer = table.getBeginPlayer();
            dealInitialCards();
          }
        };

    EventHandler<void, DealerPlayHandCmd> playDealerHandHandler =
        [this](const WrappedEvent<void, DealerPlayHandCmd>& e) {
          (void)e;
          playDealerHand();
        };

    EventHandler<void, CardResp> receiveCardHandler =
        [this](const WrappedEvent<void, CardResp>& e) {
          dealerHand->addCard(e.event.card);
        };

    EventHandler<std::reference_wrapper<const Dealer>, ToConstRefInv>
        toConstRefInvHandler =
            [this](const WrappedEvent<std::reference_wrapper<const Dealer>,
                                      ToConstRefInv>& e) -> const Dealer& {
      (void)e;
      return *this;
    };

    EventHandler<void, DealerHandleRoundResultsCmd> roundResultsHandler =
        [this](const WrappedEvent<void, DealerHandleRoundResultsCmd>& e) {
          (void)e;
          handleRoundResults();
        };

    r->listen(sourceNode, false, toConstRefInvHandler);
    r->listen(sourceNode, false, startRoundHandler);
    r->listen(sourceNode, false, playerActionHandler);
    r->listen(sourceNode, false, playDealerHandHandler);
    r->listen(sourceNode, false, receiveCardHandler);
    r->listen(sourceNode, false, roundResultsHandler);
  }
}

ConstPlayerNodeIter Dealer::getCurrPlayerNode() const { return currPlayer; };

void Dealer::dealInitialCards() {
  if (!router.expired()) {
    auto r = router.lock();
    r->broadcast(sourceNode, ShuffleIfNeededCmd<void>{});
    const auto players = r->broadcast(
        sourceNode, ToConstRefInv<std::reference_wrapper<const Player>>{});

    // deal everyone with a non-zero wager the first card
    for (auto wrappedPlayer : players) {
      const auto playerNodeId = wrappedPlayer.get().getNodeId();
      const auto wager =
          r->invoke(sourceNode, playerNodeId, PlayerGetWagerInv<Wager>{});

      if (wager && wager.value() > 0) {
        r->invoke(sourceNode, playerNodeId,
                  AdjustBankrollCmd<void>{
                      -1 * static_cast<Bankroll>(wager.value()) });
        r->invoke(sourceNode, playerNodeId,
                  PlayerStartRoundCmd<void>{ wager.value() });
        r->broadcast(sourceNode, CardReq<void>{ playerNodeId, false });
      }
    }

    // deal first card to dealer (unobservable)
    r->broadcast(sourceNode, CardReq<void>{ *sourceNode, true });

    // deal everyone with a non-zero wager the second card
    for (const auto wrappedPlayer : players) {
      const auto& player = wrappedPlayer.get();
      if (player.getCurrentHandWager() > 0) {
        r->broadcast(sourceNode, CardReq<void>{ player.getNodeId(), false });
      }
    }

    // deal second card to dealer (observable)
    r->broadcast(sourceNode, CardReq<void>{ *sourceNode, false });
    const auto upCard = *(std::next(dealerHand->getBeginIter()));
    r->broadcast(sourceNode, PlayerReceiveUpCardCmd<void>{ upCard });
  }
}

bool Dealer::checkDealerBlackjack() const { return dealerHand->isBlackjack(); }

NodeId Dealer::getNodeId() const { return *sourceNode; };

ConstHandIter Dealer::getDealerHand() const { return dealerHand; };

bool Dealer::shouldPlayDealerHand() {
  if (auto r = router.lock()) {
    const auto players = r->broadcast(
        sourceNode, ToConstRefInv<std::reference_wrapper<const Player>>{});

    for (const auto wrappedPlayer : players) {
      const auto& player = wrappedPlayer.get();

      const auto handOpt = player.getBeginHand();
      const auto endHandOpt = player.getEndHand();
      if (handOpt && endHandOpt) {
        for (auto hand = handOpt.value(), endHand = endHandOpt.value();
             hand != endHand; ++hand) {
          if (!hand->isBusted() && !hand->isBlackjack()) {
            return true;
          }
        }
      }
    }
  }

  return false;
}

void Dealer::playDealerHand() {
  if (auto r = router.lock()) {
    // allow players to observe the dealer's down card (they couldn't before)
    // this assumes the dealer hides their first card, shows their second
    const auto dealerDownCard = *dealerHand->getBeginIter();
    r->invoke(sourceNode, *sourceNode, CardResp<void>{ dealerDownCard });

    if (shouldPlayDealerHand()) {
      const auto tableOpt = r->invokeFirstAvailable(
          sourceNode, ToConstRefInv<std::reference_wrapper<const Table>>{});

      if (tableOpt) {
        const auto& table = tableOpt.value().get();
        const auto shouldHitSoft17 = table.shouldDealerHitSoft17();
        while (dealerHand->getValue() <= DEALER_STAND_VALUE) {
          if (dealerHand->getValue() < DEALER_STAND_VALUE) {
            r->broadcast(sourceNode, CardReq<void>{ *sourceNode, false });
          } else if (shouldHitSoft17 && dealerHand->isSoft()) {
            r->broadcast(sourceNode, CardReq<void>{ *sourceNode, false });
          } else { // dealer has soft 17 and stands
            break;
          }
        }
      }
    }
  }
}

void Dealer::handleRoundResults() {
  if (auto r = router.lock()) {
    const auto tableOpt = r->invokeFirstAvailable(
        sourceNode, ToConstRefInv<std::reference_wrapper<const Table>>{});

    if (tableOpt) {
      const auto& table = tableOpt.value().get();
      const auto blackjackPayoutRatio = table.getBlackjackPayoutRatio();

      const auto playerRefs = r->broadcast(
          sourceNode, ToConstRefInv<std::reference_wrapper<const Player>>{});

      for (const auto wrappedPlayer : playerRefs) {
        const auto& player = wrappedPlayer.get();
        const auto handOpt = player.getBeginHand();
        const auto endHandOpt = player.getEndHand();
        if (handOpt && endHandOpt) {
          for (auto hand = handOpt.value(), endHand = endHandOpt.value();
               hand != endHand; ++hand) {
            const auto handValue = hand->getValue();
            const auto handWager = hand->getWager();
            const auto dealerHandValue = dealerHand->getValue();
            const auto playerBusted = hand->isBusted();
            const auto dealerBusted = dealerHand->isBusted();

            Bankroll payout = 0;
            if (hand->isBlackjack() && !hand->isSplit() &&
                !dealerHand->isBlackjack()) {
              // pay blackjack payout + original wager
              payout = (1.0 + blackjackPayoutRatio) * handWager;
            } else if (handValue == dealerHandValue) {
              // push, just re-credit what we debitted earlier
              payout = handWager;
            } else if ((dealerBusted && !playerBusted) ||
                       (!playerBusted && handValue > dealerHandValue)) {
              payout = 2 * handWager;
            } // else -> player lost, do nothing

            if (payout > 0) {
              r->invoke(sourceNode, player.getNodeId(),
                        AdjustBankrollCmd<void>{ payout });
            }
          }
        }
      }
    }
  }
}
