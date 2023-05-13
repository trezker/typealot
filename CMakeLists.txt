cmake_minimum_required(VERSION 3.16)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(typealot)
file(GLOB typealot_SRC "src/*.h" "src/*.c")
add_executable(typealot ${typealot_SRC})
target_compile_options(typealot PRIVATE
  -Winline -Wunused -Wall -Wextra -Wshadow -Wcast-align -Wpedantic -Werror
)
target_link_libraries(typealot)
