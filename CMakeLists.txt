cmake_minimum_required (VERSION 3.11)
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG 1f6e110
  GIT_PROGRESS   TRUE
  EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(ftxui)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)

project(kernel-editor LANGUAGES CXX VERSION 1.0.0)
add_executable(kernel-editor src/main.cpp src/cpu.cpp)
target_link_libraries(kernel-editor
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  fmt::fmt
)
