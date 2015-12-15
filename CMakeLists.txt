cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

set(TARGET ludum_dare_34)
project(${TARGET})

set(SRC )

# includes

#if windows
if (WIN32)
    set(SRC ${SRC} src/jsoncpp.cpp )
    set(SFML_ROOT "C:/Program Files (x86)/SFML")
    find_package(SFML COMPONENTS audio graphics window system)
    include_directories(${SFML_INCLUDE_DIR})
    include_directories("E:/dev/downloads/jsoncpp/dist")
    include_directories("E:/dev/usr/include")
endif(WIN32)

# if linux
if (UNIX)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
    find_package(Jsoncpp)
    include_directories(${JSONCPP_INCLUDE_DIR})
    find_package(SFML COMPONENTS audio graphics window system)
    include_directories(${SFML_INCLUDE_DIR})
endif(UNIX)

# enable c++11
set(CMAKE_CXX_FLAGS "-std=c++0x")
set(CMAKE_BUILD_TYPE "Release")

# source files
set(SRC ${SRC}
	src/main.cpp
	src/loader.cpp
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

# libraries
set(LIBS
	${JSONCPP_LIBRARY}
	${SFML_LIBRARIES}
	${SFML_DEPENDENCIES}
)

add_executable(${TARGET} ${SRC})
target_link_libraries(${TARGET} ${LIBS})
