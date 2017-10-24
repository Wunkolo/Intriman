cmake_minimum_required( VERSION 3.2.2 )
project( Intriman )

### Standard
set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

### Verbosity
set( CMAKE_COLOR_MAKEFILE ON )
set( CMAKE_VERBOSE_MAKEFILE ON )

### Optimizations
if( MSVC )
	add_compile_options( /W3 )
elseif( CMAKE_COMPILER_IS_GNUCXX )
	add_compile_options( -m64 )
	add_compile_options( -march=native )
	add_compile_options( -Ofast )
	add_compile_options( -Wall )
	add_compile_options( -Wextra )
endif()

### Targets
add_executable(
	Intriman
	source/main.cpp

### pugixml
	dependencies/pugixml/src/pugiconfig.hpp
	dependencies/pugixml/src/pugixml.hpp
	dependencies/pugixml/src/pugixml.cpp
)
target_include_directories(
	Intriman
	PRIVATE
	include
	dependencies/pugixml/src
)
