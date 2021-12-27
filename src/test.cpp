#include "card.h"
#include <iostream>

using namespace bjcard;
int main (int argc, char** argv) {
  (void) argc;
  (void) argv;
  for (auto i = 0U; i < std::variant_size_v<Card>; ++i) {
    std::cout << indexToCard(i)  << std::endl;
  }
  // std::cout << Card { std::variant_alternative_t<0, Card>() } << std::endl;
  // std::cout << Card { std::variant_alternative_t<1, Card>() } << std::endl;
  // std::cout << Card { std::variant_alternative_t<2, Card>() } << std::endl;
  // std::cout << indexToCard(1)  << std::endl;
  // std::cout << indexToCard(2)  << std::endl;
}

