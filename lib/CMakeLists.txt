file(GLOB SRC
    "*.cpp")

if(BUILD_SHARED_LIBS)
    add_library(LIB_NAME SHARED)
    add_library(LIB_NAME::shared ALIAS LIB_NAME)
else()
    add_library(LIB_NAME STATIC)
    add_library(LIB_NAME::static ALIAS LIB_NAME)
endif()
    
target_sources(LIB_NAME 
    PRIVATE ${SRC})

target_include_directories(LIB_NAME
    PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(LIB_NAME)

set_target_properties(LIB_NAME
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED TRUE)

if(CMAKE_BUILD_TYPE STREQUAL Release)
    set_target_properties(LIB_NAME
        PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(CLANG_TIDY)
    set_target_properties(LIB_NAME PROPERTIES
        CXX_CLANG_TIDY "clang-tidy;-header-filter=./include")
endif()

if(MSVC)
    target_compile_options(LIB_NAME
        PRIVATE /W4)
else()
    target_compile_options(LIB_NAME
        PRIVATE -Wall
        PRIVATE -Werror
        PRIVATE -Wextra)
endif()
