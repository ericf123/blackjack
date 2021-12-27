#include "card.h"
#include <iostream>

using namespace bjcard;
int main (int argc, char** argv) {
  (void) argc;
  (void) argv;
  for (auto i = 0U; i < std::variant_size_v<Card>; ++i) {
    std::cout << indexToCard(i)  << std::endl;
  }

  std::cout << "----------" << std::endl;

  std::vector<Card> testVec { indexToCard(0), indexToCard(13), indexToCard(7) };
  std::cout << "total: " << sumCards(testVec.cbegin(), testVec.cend()) << std::endl;
}

