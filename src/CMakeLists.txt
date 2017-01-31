cmake_minimum_required(VERSION 2.8)
SET (CMAKE_VERBOSE_MAKEFILE 1)

add_executable (welcome welcome_c.cc)
target_link_libraries( welcome rt pthread)