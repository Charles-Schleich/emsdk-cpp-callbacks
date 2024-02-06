
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

# func-wrapper Adds a JS-Callback to be passed to C to be used. 
# TODO more functions exposed via Exported_functions
set(WASM_LINK_FLAGS "--js-library=${CMAKE_SOURCE_DIR}/func-wrapper.js \
--bind \
-s TEXTDECODER=0 \
-s ENVIRONMENT=web,worker \
-s EXPORTED_RUNTIME_METHODS=[\"writeArrayToMemory\",\"getValue\",\"setValue\",\"registerJSCallback\",\"cwrap\",\"UTF8ToString\",\"stringToUTF8OnStack\"] \
-s EXPORTED_FUNCTIONS=['_test_call','_test_call_no_args'] \
-s PTHREAD_POOL_SIZE=16 \
-s ALLOW_TABLE_GROWTH \
-s RESERVED_FUNCTION_POINTERS=100 \
-s ASYNCIFY=1 \
-s ASYNCIFY_IMPORTS=[\"ts_callback\"] \
-s ASYNCIFY_STACK_SIZE=65535 \
-s EXPORT_ES6=1 \
-s MODULARIZE=1 \
-s USE_ES6_IMPORT_META=1 \
-s LINKABLE=1 \
-s EXPORT_ALL=1 \
-s EXPORT_NAME=cppWasm \
-s ALLOW_TABLE_GROWTH \
-pthread \
-fPIC ")

# -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
# TODO: Get exported Functions flag to work later
# Cannot Export Types for C files
# Need to figure out how to emit types for C files
#  --embind-emit-tsd =${CMAKE_SOURCE_DIR}/cpp-wasm.d.ts \
# can emit types for C++, add C++ interface file to make emitting types easier ?
# set(TYPE_GENERATION_FLAG "-lembind --embind-emit-tsd /src/cpp-wasm/build.emscripten/cpp-wasm.d.ts")

add_executable(cpp-wasm src/cppwasm.cpp)
set_target_properties(cpp-wasm PROPERTIES LINK_FLAGS ${WASM_LINK_FLAGS} )
set_target_properties (cpp-wasm PROPERTIES COMPILE_FLAGS ${WASM_BUILD_FLAGS})