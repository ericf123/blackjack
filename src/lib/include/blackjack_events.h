// TODO: separate tui events from lib events
#pragma once
#include "card.h"
#include "player.h"
#include "player_action.h"
#include "table.h"

struct AdjustBankrollCmd {
  Bankroll changeAmount;
};

struct CardReq {
  const NodeId& receiver;
  bool secret;
};

struct CardResp {
  const bjcard::Card& card;
};

struct DealerPlayHandCmd {};
struct DrawToScreenCmd {};

struct InputPlayerActionInv {};

struct InputBlockUntilKeyPressed {};

struct InputGetAndMapKeyPress {};
struct PlayerActionCmd {
  const PlayerAction& action;
};

struct PlayerDoubleHandCmd {};

struct PlayerEndHandCmd {};

struct PlayerGetWagerInv {};

struct PlayerReceiveUpCardCmd {
  const bjcard::Card& card;
};

struct PlayerSplitHandCmd {};

struct PlayerStartRoundCmd {
  const Wager& wager;
};

struct StartRoundCmd {};

struct ShuffleIfNeededCmd {};

struct TableBeginPlayerReq {};

struct TableBeginPlayerResp {
  PlayerNodeIter player;
};

struct TableEndPlayerReq {};

struct TableEndPlayerResp {
  PlayerNodeIter player;
};

struct TableViewDealerDownCardVisCmd {
  bool visibile;
};

template <typename T> struct ToConstRefInv {};

struct TuiPlayerActionCmd {
  const PlayerAction& action;
};

struct ViewUpdateCmd {};

struct WagerViewGetWagerInv {
  const Wager& minWager;
  const Wager& maxWager;
};
