cmake_minimum_required(VERSION 3.26)
PROJECT(select-test)
set(CMAKE_CXX_STANDARD 11)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(APP_NAME_1 server)
set(APP_NAME_2 client)

add_executable(${APP_NAME_1} selectServer.c)

add_executable(${APP_NAME_2} client.c)
