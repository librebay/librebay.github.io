cmake_minimum_required(VERSION 3.5)

project(md2html LANGUAGES C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

include_directories(${GLIB_INCLUDE_DIRS})
message(staus "GLIB_INCLUDE_DIRS = ${GLIB_INCLUDE_DIRS}")

link_directories(${GLIB_LIBRARY_DIRS})
message(staus "GLIB_LIBRARY_DIRS = ${GLIB_LIBRARY_DIRS}")

add_definitions(${GLIB_CFLAGS_OTHER})
message(status " GLIB_CFLAGS_OTHER = ${GLIB_CFLAGS_OTHER}")

add_executable(${CMAKE_PROJECT_NAME} md2html.c)
target_link_libraries(${CMAKE_PROJECT_NAME} ${GLIB_LIBRARIES})

message(status "GTK3_LIBRARIES = ${GTK3_LIBRARIES}")
message(status "pkgs_cflags = ${PKGS_CFLAGS}")
