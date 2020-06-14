cmake_minimum_required(VERSION 3.10)

project(c2pconvert VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(TARGET c2pconvert)
set(source src/util.cpp src/c2p.cpp src/main.cpp)

add_executable("${TARGET}" ${source})

find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

target_compile_options(${TARGET} PRIVATE -Wall -Wextra -pedantic -Werror)

include_directories(include ${PNG_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})

target_link_libraries(${TARGET} ${PNG_LIBRARIES} ${ZLIB_LIBRARIES})

install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR})
