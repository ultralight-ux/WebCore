cmake_minimum_required(VERSION 2.8.8)

project(SQLite)

set_property(DIRECTORY . PROPERTY FOLDER "Deps/SQLite")

include_directories("${SQLITE_DIR}")

if (PORT MATCHES "UltralightLinux")
add_definitions(-fPIC)
endif()

add_library(sqlite3 sqlite3.c)
