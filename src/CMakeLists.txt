
add_executable(devitka cursor.c event_trace.c main.c edda.c config.c list.c)


target_link_libraries(devitka yaml)

