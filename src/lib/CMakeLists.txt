add_subdirectory(event)

add_library(blackjack STATIC dealer.cpp shoe.cpp hand.cpp hi_lo.cpp table.cpp player.cpp)
               
target_include_directories(blackjack PUBLIC include)