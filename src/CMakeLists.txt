set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE debug)
add_executable(blackjack blackjack.cpp dealer.cpp deck.cpp hand.cpp table.cpp 
               player/player.cpp player/cli_player.cpp)