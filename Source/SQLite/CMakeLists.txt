cmake_minimum_required(VERSION 3.15.0)

project(SQLite)

set_property(DIRECTORY . PROPERTY FOLDER "Deps/SQLite")

include_directories("${SQLITE_DIR}")

if (PORT MATCHES "UltralightLinux")
add_definitions(-fPIC)
endif()

add_library(sqlite3 sqlite3.c)
