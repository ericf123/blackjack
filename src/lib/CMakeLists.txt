add_library(blackjack STATIC dealer.cpp shoe.cpp hand.cpp table.cpp player.cpp)
               
target_include_directories(blackjack PUBLIC include)