file(GLOB SRC
    "*.cpp")

if(BUILD_SHARED_LIBS)
    add_library(ElderLISP SHARED)
    add_library(ElderLISP::shared ALIAS ElderLISP)
else()
    add_library(ElderLISP STATIC)
    add_library(ElderLISP::static ALIAS ElderLISP)
endif()
    
target_sources(ElderLISP 
    PRIVATE ${SRC})

target_include_directories(ElderLISP
    PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(ElderLISP
    PRIVATE ctre::ctre)

set_target_properties(ElderLISP
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED TRUE)

if(CMAKE_BUILD_TYPE STREQUAL Release AND LTO)
    set_target_properties(ElderLISP
        PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(CLANG_TIDY)
    set_target_properties(ElderLISP PROPERTIES
        CXX_CLANG_TIDY "clang-tidy;-header-filter=./include")
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
