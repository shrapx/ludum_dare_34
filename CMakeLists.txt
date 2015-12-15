cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

set(TARGET ludum_dare_34)
project(${TARGET})

# includes
set(SFML_ROOT "C:/Program Files (x86)/SFML")
find_package(SFML COMPONENTS audio graphics window system)
include_directories(${SFML_INCLUDE_DIR})
include_directories("E:/dev/downloads/jsoncpp/dist")
include_directories("E:/dev/usr/include")

# libraries
set(LIBS
	${SFML_LIBRARIES}
	${SFML_DEPENDENCIES}
)

# source files
set(SRC
	src/main.cpp
	src/loader.cpp
	src/jsoncpp.cpp
	src/input.hpp
	src/timing.hpp
	src/LD34.hpp
	src/guy.hpp
	src/main_menu.hpp
	src/level.hpp
	src/scores.hpp
	src/colour.hpp
	src/particles.hpp
	src/sectors.hpp
	src/world.hpp
)

add_executable(${TARGET} ${SRC})
target_link_libraries(${TARGET} ${LIBS})
