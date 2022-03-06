
cmake_minimum_required(VERSION 3.21)

project(iconv VERSION 0.1 LANGUAGES CXX)

add_library(iconv STATIC)

set_target_properties(iconv PROPERTIES DEBUG_POSTFIX "d")

target_sources(
    iconv
    PRIVATE
    iconv.cpp
)

set_property(
    TARGET iconv
    PROPERTY PUBLIC_HEADER
    iconv.h
)

install(
    TARGETS iconv
    EXPORT libiconv
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)
