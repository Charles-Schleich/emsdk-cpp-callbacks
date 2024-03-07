
cmake_minimum_required(VERSION 3.10)

project(cppjs)
set(CMAKE_C_STANDARD 11)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build.emscripten)

# PICO
set(BUILD_EXAMPLES OFF CACHE BOOL "Disables examples" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "DIsables tests" FORCE)
set(BUILD_MULTICAST OFF CACHE BOOL "Disables multicast tests" FORCE)
set(BUILT_INTEGRATION OFF CACHE BOOL "Disables integration tests " FORCE)
set(BUILD_TOOLS OFF CACHE BOOL "Disables build tools" FORCE)


set(CMAKE_EXECUTABLE_SUFFIX ".js")

set(WASM_BUILD_FLAGS " \
-pthread \
-fPIC")

set(WASM_LINK_FLAGS "--bind \
-s ENVIRONMENT=web,worker \
-s EXPORTED_RUNTIME_METHODS=['writeArrayToMemory','cwrap'] \
-s EXPORTED_FUNCTIONS=['_malloc','addFunction'] \
-s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE=$addFunction \
-s PTHREAD_POOL_SIZE=1 \
-s ALLOW_TABLE_GROWTH \
-s RESERVED_FUNCTION_POINTERS=100 \
-s ASYNCIFY=1 \
-s ASYNCIFY_IMPORTS=['ts_callback','async_ts_callback'] \
-s ASYNCIFY_STACK_SIZE=65535 \
-s EXPORT_ES6=1 \
-s MODULARIZE=1 \
-s USE_ES6_IMPORT_META=1 \
-s LINKABLE=1 \
-s EXPORT_ALL=1 \
-s EXPORT_NAME=cppWasm \
-s ALLOW_TABLE_GROWTH \
-s RESERVED_FUNCTION_POINTERS=20 \
-pthread \
-fPIC \
-lembind")

add_executable(cpp-wasm src/cppwasm.cpp)
set_target_properties(cpp-wasm PROPERTIES LINK_FLAGS ${WASM_LINK_FLAGS} )
set_target_properties (cpp-wasm PROPERTIES COMPILE_FLAGS ${WASM_BUILD_FLAGS})