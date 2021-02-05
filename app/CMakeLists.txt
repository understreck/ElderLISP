add_executable(ElderInterp main.cpp)

if(BUILD_SHARED_LIBS)
    target_link_libraries(ElderInterp
        PRIVATE ElderLISP::shared)
else()
    target_link_libraries(ElderInterp
        PRIVATE ElderLISP::static)
endif()

set_target_properties(ElderInterp
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED TRUE)

if(CMAKE_BUILD_TYPE STREQUAL Release AND LTO)
    set_target_properties(ElderInterp
        PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(MSVC)
    target_compile_options(ElderInterp
        PRIVATE /W4)
else()
    target_compile_options(ElderInterp
        PRIVATE -Wall
        PRIVATE -Werror
        PRIVATE -Wextra)
endif()

if(WIN32 AND NOT WIN32CONSOLE)
  target_link_options(ElderInterp
        PRIVATE /SUBSYSTEM:WINDOWS
        PRIVATE /ENTRY:mainCRTStartup)
endif()
