cmake_minimum_required(VERSION 3.0)
project(levenshteinsgate)

add_library(levenshteinsgate INTERFACE)
target_sources(levenshteinsgate INTERFACE
    include/levenshteinsgate/trie.h)
target_include_directories(levenshteinsgate INTERFACE
    include/)
install(FILES
    include/levenshteinsgate/trie.h
    DESTINATION include/levenshteinsgate/)
