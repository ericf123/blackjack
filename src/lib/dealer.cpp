#include "dealer.h"
#include "blackjack_events.h"
#include "event_def.h"
#include "event_router.h"
#include "table.h"
#include <functional>
#include <memory>
#include <optional>

Dealer::Dealer(std::weak_ptr<EventRouter> router, OwningHandle sourceNode,
               const NodeId& tableNode)
    : sourceNode(sourceNode), tableNode(tableNode), router(router) {
  // TODO: round end handler
  dealerHands.push_front(Hand());
  dealerHand = dealerHands.begin();

  if (auto r = router.lock()) {
    // TODO: remove in favor of invoke
    EventHandler<TableEndPlayerResp> endPlayerHandler =
        [this](const WrappedEvent<TableEndPlayerResp>& e) {
          endPlayer = e.event.player;
        };

    EventHandler<TableBeginPlayerResp> beginPlayerHandler =
        [this](const WrappedEvent<TableBeginPlayerResp>& e) {
          beginPlayer = e.event.player;
        };

    EventHandler<PlayerActionCmd> playerActionHandler =
        [this](const WrappedEvent<PlayerActionCmd>& e) {
          if (currPlayer != endPlayer && e.sourceId == *currPlayer) {
            const auto playerOpt =
                e.router.invoke<std::reference_wrapper<const Player>>(
                    this->sourceNode, e.sourceId, ToConstRefInv<Player>{});
            if (playerOpt) {
              const auto& player = playerOpt.value().get();
              auto action = e.event.action;

              auto drawCard = action == PlayerAction::Hit;

              if (action == PlayerAction::Split) {
                const auto debitAmount = -1 * player.getCurrentHandWager();
                e.router.send(
                    this->sourceNode, *currPlayer,
                    AdjustBankrollCmd{ static_cast<Bankroll>(debitAmount) });
                e.router.send(this->sourceNode, *currPlayer,
                              PlayerSplitHandCmd{});
              }

              if (action == PlayerAction::DoubleDown) {
                const auto debitAmount = -1 * player.getCurrentHandWager();
                e.router.send(
                    this->sourceNode, *currPlayer,
                    AdjustBankrollCmd{ static_cast<Bankroll>(debitAmount) });
                e.router.send(this->sourceNode, *currPlayer,
                              PlayerDoubleHandCmd{});

                drawCard = true;

                if (player.playingLastHand()) {
                  action = PlayerAction::EndTurn;
                } else {
                  action = PlayerAction::Stand;
                }
              }

              // deal player a card if necessary
              if (drawCard) {
                e.router.send(this->sourceNode, this->tableNode,
                              CardReq{ *currPlayer, false });
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
                e.router.send(this->sourceNode, *currPlayer,
                              PlayerEndHandCmd{});
              }

              if (action == PlayerAction::EndTurn) {
                currPlayer = std::next(currPlayer);
              }
            }
          }
        };

    EventHandler<StartRoundCmd> startRoundHandler =
        [this](const WrappedEvent<StartRoundCmd>& e) {
          (void)e;
          dealerHand->reset();

          const auto tableOpt =
              e.router
                  .invokeFirstAvailable<std::reference_wrapper<const Table>>(
                      this->sourceNode, ToConstRefInv<Table>{});
          if (tableOpt) {
            const auto& table = tableOpt.value().get();
            beginPlayer = table.getBeginPlayer();
            currPlayer = table.getBeginPlayer();
            endPlayer = table.getEndPlayer();
            dealInitialCards();
          }
        };

    EventHandler<DealerPlayHandCmd> playDealerHandHandler =
        [this](const WrappedEvent<DealerPlayHandCmd>& e) {
          (void)e;
          playDealerHand();
        };

    EventHandler<CardResp> receiveCardHandler =
        [this](const WrappedEvent<CardResp>& e) {
          dealerHand->addCard(e.event.card);
        };

    InvokeHandler<std::reference_wrapper<const Dealer>, ToConstRefInv<Dealer>>
        toConstRefInvHandler =
            [this](
                const WrappedEvent<ToConstRefInv<Dealer>>& e) -> const Dealer& {
      (void)e;
      return *this;
    };

    r->registerInvokeHandler(sourceNode, toConstRefInvHandler);
    // register the player action handler
    r->listen(sourceNode, false, startRoundHandler);
    r->listen(sourceNode, false, playerActionHandler);

    // register play dealer hand handler
    r->listen(sourceNode, false, playDealerHandHandler);
    r->listen(sourceNode, false, receiveCardHandler);
  }
}

ConstPlayerNodeIter Dealer::getCurrPlayerNode() const { return currPlayer; };

void Dealer::dealInitialCards() {
  if (!router.expired()) {
    auto r = router.lock();
    r->send(sourceNode, tableNode, ShuffleIfNeededCmd{});
    r->send(sourceNode, tableNode, TableBeginPlayerReq{});

    // deal everyone with a non-zero wager the first card
    for (auto playerIter = beginPlayer; playerIter != endPlayer; ++playerIter) {
      const auto wager =
          r->invoke<Wager>(sourceNode, *playerIter, PlayerGetWagerInv{});

      if (wager && wager.value() > 0) {
        r->send(sourceNode, *playerIter,
                AdjustBankrollCmd{ -1 * static_cast<Bankroll>(wager.value()) });
        r->send(sourceNode, *playerIter, PlayerStartRoundCmd{ wager.value() });
        r->send(sourceNode, tableNode, CardReq{ *playerIter, false });
      }
    }

    // deal first card to dealer (unobservable)
    // dealerHand->addCard(table->drawCard(false));
    r->send(sourceNode, tableNode, CardReq{ *sourceNode, true });
    // deal everyone with a non-zero wager the second card
    // TODO: replace with broadcasting ToConstRefInv<Player>
    for (auto playerIter = beginPlayer; playerIter != endPlayer; ++playerIter) {
      const auto playerOpt = r->invoke<std::reference_wrapper<const Player>>(
          sourceNode, *playerIter, ToConstRefInv<Player>{});
      if (playerOpt) {
        const auto& player = playerOpt.value().get();
        if (player.getCurrentHandWager() > 0) {
          r->send(sourceNode, tableNode, CardReq{ *playerIter, false });
        }
      }
    }

    // deal second card to dealer (observable)
    r->send(sourceNode, tableNode, CardReq{ *sourceNode, false });
    const auto upCard = *(std::next(dealerHand->getBeginIter()));
    r->broadcast(sourceNode, PlayerReceiveUpCardCmd{ upCard });
  }
}

bool Dealer::checkDealerBlackjack() const { return dealerHand->isBlackjack(); }

NodeId Dealer::getNodeId() const { return *sourceNode; };

ConstHandIter Dealer::getDealerHand() const { return dealerHand; };

bool Dealer::shouldPlayDealerHand() {
  if (!router.expired()) {
    auto r = router.lock();
    // TODO: replace with broadcasting ToConstRefInv<Player>
    for (auto playerIter = beginPlayer; playerIter != endPlayer; ++playerIter) {
      const auto playerOpt = r->invoke<std::reference_wrapper<const Player>>(
          sourceNode, *playerIter, ToConstRefInv<Player>{});
      if (playerOpt) {
        const auto& player = playerOpt.value().get();

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
  }

  return false;
}

void Dealer::playDealerHand() {
  if (!router.expired()) {
    auto r = router.lock();

    // allow players to observe the dealer's down card (they couldn't before)
    // this assumes the dealer hides their first card, shows their second
    const auto dealerDownCard = *dealerHand->getBeginIter();
    r->send(sourceNode, *sourceNode, CardResp{ dealerDownCard });

    if (shouldPlayDealerHand()) {
      // TODO: replace with broadcasting ToConstRefInv<Table>
      const auto tableOpt = r->invoke<std::reference_wrapper<const Table>>(
          sourceNode, tableNode, ToConstRefInv<Table>{});
      if (tableOpt) {
        const auto& table = tableOpt.value().get();
        const auto shouldHitSoft17 = table.shouldDealerHitSoft17();
        while (dealerHand->getValue() <= DEALER_STAND_VALUE) {
          if (dealerHand->getValue() < DEALER_STAND_VALUE) {
            r->send(sourceNode, tableNode, CardReq{ *sourceNode, false });
          } else if (shouldHitSoft17 && dealerHand->isSoft()) {
            r->send(sourceNode, tableNode, CardReq{ *sourceNode, false });
          } else { // dealer has soft 17 and stands
            break;
          }
        }
      }
    }
  }
}

void Dealer::handleRoundResults() {
  if (!router.expired()) {
    auto r = router.lock();
    // TODO: replace with broadcasting ToConstRefInv<Table>
    const auto tableOpt = r->invoke<std::reference_wrapper<const Table>>(
        sourceNode, tableNode, ToConstRefInv<Table>{});
    if (tableOpt) {
      const auto& table = tableOpt.value().get();
      const auto blackjackPayoutRatio = table.getBlackjackPayoutRatio();

      // TODO: replace with broadcasting ToConstRefInv<Player>
      const auto playerRefs =
          r->broadcastInvoke<std::reference_wrapper<const Player>>(
              sourceNode, ToConstRefInv<Player>{});

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
              r->send(sourceNode, player.getNodeId(),
                      AdjustBankrollCmd{ payout });
            }
          }
        }
      }
    }
  }
}
