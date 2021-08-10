add_library(observer STATIC observer.h)

set_target_properties(observer PROPERTIES LINKER_LANGUAGE CXX)
if(MSVC)
  set_target_properties(observer PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

target_include_directories(observer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_library(dp::observer ALIAS observer)
