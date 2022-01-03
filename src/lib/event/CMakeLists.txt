add_library(bjevent STATIC include/event_def.h include/event_router.h include/wrapped_event.h)
set_target_properties(bjevent PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(bjevent PUBLIC include)