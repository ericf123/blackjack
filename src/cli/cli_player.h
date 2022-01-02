#include "player.h"

class CliPlayer : public Player {
public:
  CliPlayer(double bankroll) : Player(bankroll) {}
  virtual ~CliPlayer() = default;
  virtual void notifyShuffle() override;
  virtual void observeCard(const Card& card) override;
  virtual PlayerAction getNextAction() override;
  virtual Wager getWager() override;

private:
  PlayerAction getDesiredAction();
  PlayerAction sanitizeAction(PlayerAction action);
  void displayHands();
};