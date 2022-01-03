add_executable(bjtui tui.cpp tui_player.cpp view/card_view.cpp 
                     view/hand_view.cpp view/stats_view.cpp view/table_view.cpp
                     view/view.cpp view/wager_view.cpp
                     view/include/title_view.h view/include/view.h)
target_include_directories(bjtui PUBLIC include view/include)
target_link_libraries(bjtui PRIVATE bjevent)
target_link_libraries(bjtui PRIVATE blackjack)
target_link_libraries(bjtui PRIVATE panel)
target_link_libraries(bjtui PRIVATE form)
target_link_libraries(bjtui PRIVATE ncursesw)