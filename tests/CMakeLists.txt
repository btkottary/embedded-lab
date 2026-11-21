# Unit tests: built with the PC's gcc (preset "host-tests") and run by CTest.
#   cmake --preset host-tests && cmake --build --preset host-tests && ctest --preset host-tests

add_library(unity STATIC ${LAB_THIRD_PARTY}/unity/src/unity.c)
target_include_directories(unity SYSTEM PUBLIC ${LAB_THIRD_PARTY}/unity/src)

# lab_add_test(<name> SOURCES <files...> LIBS <targets...>)
function(lab_add_test name)
  cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})
  add_executable(${name} ${ARG_SOURCES})
  target_link_libraries(${name} PRIVATE unity lab_warnings ${ARG_LIBS})
  add_test(NAME ${name} COMMAND ${name})
endfunction()

lab_add_test(test_ringbuf SOURCES test_ringbuf.c LIBS ringbuf)
