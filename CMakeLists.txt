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

include(InstallRequiredSystemLibraries)
if(UNIX AND NOT APPLE)
	set(CMAKE_SYSTEM_NAME linux-${CMAKE_SYSTEM_PROCESSOR})
	set(CPACK_GENERATOR TGZ)
endif()
if(WIN32)
	set(CMAKE_SYSTEM_NAME windows-${CMAKE_SYSTEM_PROCESSOR})
	set(CPACK_GENERATOR ZIP)
endif()
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
install(FILES
	${CPACK_RESOURCE_FILE_README}
	${CPACK_RESOURCE_FILE_LICENSE}
	DESTINATION .
	COMPONENT Metadata
	EXCLUDE_FROM_ALL)
include(CPack)
