add_executable(ElderLISP
    main.cpp)

target_link_libraries(ElderLISP
    PRIVATE ctre::ctre
    PRIVATE ElderLISP::LIB)

target_include_directories(ElderLISP
    PUBLIC ${PROJECT_SOURCE_DIR}/include)

set_target_properties(ElderLISP
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED TRUE)

if(CMAKE_BUILD_TYPE STREQUAL Release AND LTO)
    set_target_properties(ElderLISP
        PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(MSVC)
    target_compile_options(ElderLISP
        PRIVATE /W4)
else()
    target_compile_options(ElderLISP
        PRIVATE -Wall
        PRIVATE -Werror
        PRIVATE -Wextra)
endif()

if(WIN32 AND NOT WIN32CONSOLE)
  target_link_options(ElderLISP
        PRIVATE /SUBSYSTEM:WINDOWS
        PRIVATE /ENTRY:mainCRTStartup)
endif()
