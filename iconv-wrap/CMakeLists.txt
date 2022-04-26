
cmake_minimum_required(VERSION 3.21)

project(libiconv VERSION 0.1 LANGUAGES CXX)

include(GNUInstallDirs)

if(MSVC)
    add_compile_options(
        $<IF:$<CONFIG:Debug>,/MTd,/MT>
    )
endif()

add_library(libiconv STATIC)

target_sources(
    libiconv
    PRIVATE
    iconv.cpp
)

set_property(
    TARGET libiconv
    PROPERTY PUBLIC_HEADER
    iconv.h
)

install(
    TARGETS libiconv
    EXPORT libiconv
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

set_target_properties(libiconv PROPERTIES VERSION ${CMAKE_PROJECT_VERSION})
set_target_properties(libiconv PROPERTIES INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set_target_properties(libiconv PROPERTIES INSTALL_FULL_BINDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
set_target_properties(libiconv PROPERTIES INSTALL_FULL_LIBDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set_target_properties(libiconv PROPERTIES INSTALL_FULL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")

file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/libiconv.$<CONFIG>.pc"
    INPUT "${CMAKE_CURRENT_SOURCE_DIR}/libiconv.pc.cmakein"
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libiconv.$<CONFIG>.pc
    DESTINATION lib/pkgconfig
    RENAME libiconv.pc
)
