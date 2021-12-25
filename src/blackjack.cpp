#include <iostream>
#include <optional>

#include "card.h"
#include "dealer.h"
#include "deck.h"
#include "player/cli_player.h"

int main(int argc, char** argv) {
  Dealer dealer { std::make_unique<Table>(6, 1.5, true) };
  
  dealer.addPlayerToTable(std::make_unique<CliPlayer>(1000));

  while (true) {
    dealer.playRound();
  }
}