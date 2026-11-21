cmake_minimum_required(VERSION 3.25)
project(LudotDocsRuntime LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(LUAU_BUILD_CLI OFF CACHE BOOL "" FORCE)
set(LUAU_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LUAU_BUILD_WEB ON CACHE BOOL "" FORCE)
add_subdirectory(../../luau luau)
target_compile_definitions(Luau.Ast PRIVATE LUAU_ENABLE_ASSERT)
target_compile_definitions(Luau.Compiler PRIVATE LUAU_ENABLE_ASSERT)
target_link_options(Luau.Web PRIVATE
	-sMODULARIZE=1
	-sEXPORT_ES6=1
	-sENVIRONMENT=worker,node
	-sALLOW_MEMORY_GROWTH=1
	-sMAXIMUM_MEMORY=268435456
	-sFILESYSTEM=0
	-sSTACK_SIZE=4194304
)
set_target_properties(Luau.Web PROPERTIES OUTPUT_NAME ludot SUFFIX .mjs)
target_sources(Luau.Web PRIVATE intellisense.cpp)
target_include_directories(Luau.Web PRIVATE ../../luau/CLI/src)
target_link_options(Luau.Web PRIVATE -sEXPORTED_FUNCTIONS=["_executeScript","_checkScript","_queryScript"])
