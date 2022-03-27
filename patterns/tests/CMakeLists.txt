include(add_test.cmake)

if(DP_BUILD_OBSERVER)
  add_test(NAME test_observer SOURCES test_observer.cpp)
endif()
