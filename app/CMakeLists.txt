add_executable(APP_NAME main.cpp)

if(BUILD_SHARED_LIBS)
    target_link_libraries(APP_NAME
        PRIVATE LIB_NAME::shared)
else()
    target_link_libraries(APP_NAME
        PRIVATE LIB_NAME::static)
endif()

set_target_properties(APP_NAME
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED TRUE)

if(CMAKE_BUILD_TYPE STREQUAL Release AND LTO)
    set_target_properties(APP_NAME
        PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(MSVC)
    target_compile_options(APP_NAME
        PRIVATE /W4)
else()
    target_compile_options(APP_NAME
        PRIVATE -Wall
        PRIVATE -Werror
        PRIVATE -Wextra)
endif()

if(WIN32 AND NOT WIN32CONSOLE)
  target_link_options(APP_NAME
        PRIVATE /SUBSYSTEM:WINDOWS
        PRIVATE /ENTRY:mainCRTStartup)
endif()
