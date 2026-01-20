cmake_minimum_required(VERSION 4.0)

add_executable(assignment1)

target_sources(assignment1 PRIVATE
    main.cpp
)

target_compile_options(assignment1 PRIVATE
    -O0
    -g3
    -fsanitize=address
    -fsanitize=undefined
)

target_link_options(assignment1 PRIVATE
    -fsanitize=address
    -fsanitize=undefined
)

