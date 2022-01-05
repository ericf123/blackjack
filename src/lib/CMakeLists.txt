add_subdirectory(event)

add_library(blackjack STATIC table.cpp dealer.cpp shoe.cpp hand.cpp hi_lo.cpp  player.cpp)
               
target_include_directories(blackjack PUBLIC include)
target_link_libraries(blackjack PUBLIC bjevent)