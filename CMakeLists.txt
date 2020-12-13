cmake_minimum_required(VERSION 3.10)

project(Bplus)

include_directories(include)

set(CMAKE_CXX_STANDARD 20)

set(HEADERS

)
set(SOURCES 
    src/main.cpp
)


add_executable(bplus ${SOURCES} ${HEADERS})