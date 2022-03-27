macro(run_conan)
  # Download automatically, you can also just copy the conan.cmake file
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(
      "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD
         "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
         "${CMAKE_BINARY_DIR}/conan.cmake" LOG DOWNLOAD_STATUS)
    message("${DOWNLOAD_STATUS}")
  endif()

  include(${CMAKE_BINARY_DIR}/conan.cmake)

  conan_add_remote(NAME bintray URL https://conan.bintray.com)

  if(DP_ENABLE_TESTS)
    set(CONAN_CATCH2 "catch2/2.13.3")
  endif()

  set(CONAN_IMPORTS "bin, *.dll -> ${CMAKE_BINARY_DIR}/bin")

  conan_cmake_run(
    REQUIRES
    ${CONAN_CATCH2}
    OPTIONS
    ${CONAN_EXTRA_OPTIONS}
    IMPORTS
    ${CONAN_IMPORTS}
    NO_IMPORTS
    OUTPUT_QUIET
    BUILD
    missing
    GENERATORS
    cmake_find_package)
endmacro()
