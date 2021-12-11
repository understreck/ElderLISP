file(
  GLOB SRC
  "*.cpp"
)

add_library(ElderLISP_LIB STATIC)
add_library(ElderLISP::LIB ALIAS ElderLISP_LIB)

target_sources(ElderLISP_LIB
  PRIVATE ${SRC})

target_include_directories(
  ElderLISP_LIB
  PUBLIC ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(
  ElderLISP_LIB
  PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)

if(CMAKE_BUILD_TYPE STREQUAL Release)
  set_target_properties(
    ElderLISP_LIB
    PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION TRUE
  )
endif()

if(CLANG_TIDY)
  set_target_properties(
    ElderLISP_LIB PROPERTIES
    CXX_CLANG_TIDY "clang-tidy;-header-filter=./include"
  )
endif()

if(MSVC)
  target_compile_options(
    ElderLISP_LIB
    PRIVATE /W4
  )
else()
  target_compile_options(
    ElderLISP_LIB
    PRIVATE -Wall
    PRIVATE -Werror
    PRIVATE -Wextra
  )
endif()
