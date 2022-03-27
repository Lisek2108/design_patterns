option(DP_BUILD_OBSERVER "Build observer pattern" ON)

if(DP_BUILD_OBSERVER)
  add_subdirectory(observer)
endif()

if(DP_TESTS_ENABLED)
  add_subdirectory(tests)
endif()
