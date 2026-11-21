add_library(ringbuf STATIC src/ringbuf.c)
target_include_directories(ringbuf PUBLIC include)
target_link_libraries(ringbuf PRIVATE lab_mcu lab_warnings)
