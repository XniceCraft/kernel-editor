cmake_minimum_required (VERSION 3.11)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG cdf28903a7781f97ba94d30b79c3a4b0c97ccce7
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
