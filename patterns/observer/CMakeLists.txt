add_library(observer INTERFACE observer.h)

set_target_properties(observer PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(observer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(dp::observer ALIAS observer)
