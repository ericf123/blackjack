#include <iostream>
#include <optional>

#include "card.h"
#include "cli_player.h"
#include "dealer.h"
#include "deck.h"

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  Dealer dealer{ std::make_shared<Table>(6, 1.5, true) };

  dealer.addPlayerToTable(std::make_shared<CliPlayer>(1000));

  while (true) {
    dealer.playRound();
  }

  return 0;
}