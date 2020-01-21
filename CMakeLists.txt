cmake_minimum_required( VERSION 3.2.2 )
project( Intriman )

### Standard
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

### Verbosity
set( CMAKE_COLOR_MAKEFILE ON )
set( CMAKE_VERBOSE_MAKEFILE ON )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

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
	source/Intriman/main.cpp
	source/Intriman/Intriman.cpp
	source/Intriman/Settings.cpp
	source/Intriman/Generators/Text.cpp
	source/Intriman/Generators/Roff.cpp
	source/Intriman/Generators/Markdown.cpp

### pugixml
	dependencies/pugixml/src/pugiconfig.hpp
	dependencies/pugixml/src/pugixml.hpp
	dependencies/pugixml/src/pugixml.cpp
)

if( MSVC )

elseif( CMAKE_COMPILER_IS_GNUCXX )
	target_link_libraries(
		Intriman
		stdc++fs
	)
endif()

target_include_directories(
	Intriman
	PRIVATE
	include
	dependencies/pugixml/src
)

### Download data-latest.xml listing
message(
	STATUS "Downloading latest intrinsics data from intel..."
)
file(
	DOWNLOAD
	"https://software.intel.com/sites/landingpage/IntrinsicsGuide/files/data-latest.xml"
	"./intrinsics.xml"
	SHOW_PROGRESS
)

message(
	STATUS "Downloading latency data from uops.info..."
)
file(
	DOWNLOAD
	"https://uops.info/instructions.xml"
	"./latency.xml"
	SHOW_PROGRESS
)