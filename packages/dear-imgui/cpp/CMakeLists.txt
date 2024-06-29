cmake_minimum_required(VERSION 3.8)

set(CMAKE_GENERATOR "Ninja")
set(CMAKE_TOOLCHAIN_FILE deps/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(VCPKG_TARGET_TRIPLET "wasm32-emscripten")


if(NOT DEFINED ENV{EMSDK})
    message(FATAL_ERROR "Environment variable EMSDK is not defined. Follow the README steps and source emsdk_env.* from the EMSDK root directory in this shell before running CMake")
endif()


set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE $ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake)

execute_process(
        COMMAND "npm install"
        WORKING_DIRECTORY "$ENV{EMSDK}/upstream/emscripten"
)

project(reactDearImgui LANGUAGES C CXX)

# add_definitions(-std=c++23)
set(CMAKE_CXX_STANDARD 23)

set(MLN_WITH_CORE_ONLY ON)
set(MLN_WITH_WERROR OFF)
set(DEPS ${CMAKE_CURRENT_SOURCE_DIR}/deps)

find_package(OpenGL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(imgui REQUIRED)
find_package(implot REQUIRED)
# find_package(RPP REQUIRED)

set(CMAKE_EXECUTABLE_SUFFIX ".mjs")

if (DEFINED EMSCRIPTEN)
	add_executable(reactDearImgui main.cpp
        shared.cpp
        widget.cpp
        reactimgui.cpp
        ${DEPS}/wasm-glfw3-boilerplate/view.cpp
        ${DEPS}/wasm-glfw3-boilerplate/glwasm.cpp
        imguiview.cpp
        implotview.cpp
    )

    target_include_directories(reactDearImgui PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR} 
        ${DEPS}/wasm-glfw3-boilerplate 
        ${DEPS}/IconFontCppHeaders 
        ${DEPS}/ReactivePlusPlus/src/rpp 
        ${DEPS}/maplibre-native-fork/include 
        ${DEPS}/maplibre-native-fork/vendor/nunicode/include
        ${DEPS}/maplibre-native-fork/vendor/mapbox-base/include
        ${DEPS}/maplibre-native-fork/vendor/mapbox-base/deps/cheap-ruler-cpp/include
        ${DEPS}/maplibre-native-fork/vendor/mapbox-base/deps/geojson-vt-cpp/include
        ${DEPS}/maplibre-native-fork/vendor/mapbox-base/deps/geojson-hpp/include
        ${DEPS}/maplibre-native-fork/vendor/mapbox-base/deps/geometry.hpp/include
        ${DEPS}/maplibre-native-fork/vendor/mapbox-base/deps/shelf-pack-cpp/include
        ${DEPS}/maplibre-native-fork/vendor/mapbox-base/deps/supercluster.hpp/include
        ${DEPS}/maplibre-native-fork/vendor/mapbox-base/deps/variant/include
        # ${DEPS}/shelf-pack-cpp/include
        # ${DEPS}/unique_resource
        # ${DEPS}/mapbox-base/include 
        # ${DEPS}/maplibre-native/platform/default/include
    )

    # https://github.com/emscripten-core/emscripten/issues/18476
    # -c -fexperimental-library

	set_target_properties(reactDearImgui PROPERTIES COMPILE_FLAGS "-O0 -pthread -fPIC -Wno-error=deprecated-declarations -Wformat -s DISABLE_EXCEPTION_CATCHING=0")
    set_target_properties(reactDearImgui PROPERTIES LINK_FLAGS "--no-heap-copy -pthread -fPIC --use-preload-plugins --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/../fonts@/fonts -O0 -lembind --emit-tsd interface.d.ts -lhtml5.js -lhtml5_webgl.js -lglfw.js -s DISABLE_EXCEPTION_CATCHING=0 -s ENVIRONMENT='web,worker' -s MODULARIZE=1 -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s USE_GLFW=3 -s USE_WEBGPU=1 -s NO_EXIT_RUNTIME=0 -s STANDALONE_WASM=0 -s EXIT_RUNTIME=1 -s ASSERTIONS=1 -s STACK_OVERFLOW_CHECK=2 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -sFULL_ES3 -s DISABLE_EXCEPTION_CATCHING=0 -s SINGLE_FILE=1 -sMAIN_MODULE ${CMAKE_CURRENT_SOURCE_DIR}/deps/maplibre-native-fork/platform/emscripten/build/mbgl-core.wasm")
    set_target_properties(reactDearImgui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../ts/src/lib/wasm)
    target_link_libraries(reactDearImgui PRIVATE imgui::imgui implot::implot nlohmann_json::nlohmann_json)

    # Reinstate line below once ReactivePlusPlus 2.1.2 gets released (preferably in the vcpkg repo)
    # target_link_libraries(reactDearImgui PRIVATE imgui::imgui implot::implot nlohmann_json::nlohmann_json RPP::rpp RPP::rppqt)

    # This currently breaks custom fonts - known issue
    # target_compile_definitions(reactDearImgui PUBLIC IMGUI_USE_WCHAR32)
endif()
