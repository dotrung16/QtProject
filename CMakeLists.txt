cmake_minimum_required(VERSION 3.16)
project(QtProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

add_executable(MyApp WIN32 MACOSX_BUNDLE
	main.cpp
	
	MainWindow.h
	MainWindow.cpp
)

target_link_libraries(MyApp PRIVATE
	Qt6::Widgets
	Qt6::Multimedia
)
