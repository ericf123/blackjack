// TODO: separate tui events from lib events
#pragma once
#include "card.h"
#include "player.h"
#include "player_action.h"
#include "table.h"

template <typename Return> struct AdjustBankrollCmd { Bankroll changeAmount; };

template <typename Return> struct CardReq {
  const NodeId& receiver;
  bool secret;
};

template <typename Return> struct CardResp { const bjcard::Card& card; };

template <typename Return> struct CountNotification {
  const int rawCount;
  const int trueCount;
  const double decksRemaining;
};

template <typename Return> struct DealerPlayHandCmd {};

template <typename Return> struct DrawToScreenCmd {};

template <typename Return> struct InputPlayerActionInv {};

template <typename Return> struct InputBlockUntilKeyPressed {};

template <typename Return> struct InputGetAndMapKeyPress {};

template <typename Return> struct PlayerActionCmd {
  const PlayerAction& action;
};

template <typename Return> struct PlayerDoubleHandCmd {};

template <typename Return> struct PlayerEndHandCmd {};

template <typename Return> struct PlayerGetWagerInv {};

template <typename Return> struct PlayerReceiveUpCardCmd {
  const bjcard::Card& card;
};

template <typename Return> struct PlayerSplitHandCmd {};

template <typename Return> struct PlayerStartRoundCmd { const Wager& wager; };

template <typename Return> struct StartRoundCmd {};

template <typename Return> struct ShuffleIfNeededCmd {};
template <typename Return> struct ShuffleNotification {};

template <typename Return> struct TableViewDealerDownCardVisCmd {
  bool visibile;
};

template <typename Return> struct ToConstRefInv {};

template <typename Return> struct TuiPlayerActionCmd {
  const PlayerAction& action;
};

template <typename Return> struct ViewUpdateCmd {};

template <typename Return> struct WagerViewGetWagerInv {
  const Wager& minWager;
  const Wager& maxWager;
};
