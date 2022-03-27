include(CMakeParseArguments)

macro(add_test)
  set(oneValueArgs NAME)
  set(multiValueArgs SOURCES LIBRARIES)
  cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}"
                        ${ARGN})

  add_executable(${ARG_NAME} ${ARG_SOURCES})
  target_link_libraries(${ARG_NAME} ${ARG_LIBRARIES} Catch2::Catch2)
  target_include_directories(${ARG_NAME} PRIVATE ${DP_SOURCE_DIR})
  set_target_properties(
    ${ARG_NAME}
    PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
               LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
               RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
endmacro()
