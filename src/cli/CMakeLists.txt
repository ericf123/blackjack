add_executable(bjcli blackjack.cpp cli_player.cpp cli_player.h)
target_link_libraries(bjcli PUBLIC blackjack)