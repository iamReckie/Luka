cmake_minimum_required(VERSION 3.26)
project(Luka LANGUAGES CXX)

# CUDA 지원 추가 (옵션)
option(ENABLE_CUDA "Enable CUDA support" OFF)

if(ENABLE_CUDA)
    enable_language(CUDA)
    find_package(CUDAToolkit REQUIRED)
    message(STATUS "CUDA support enabled")
    add_compile_definitions(CUDA_ENABLED)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add sub directory
add_subdirectory(libs/OpenXLSX)
add_subdirectory(libs/yaml-cpp)

file(GLOB DATA_PROCESSOR_SOURCES
src/DataProcessor/*.cc
src/DataProcessor/*.h
)
file(GLOB COMMAND_PROCESSOR_SOURCES
src/CommandProcessor/*.cc
src/CommandProcessor/*.h
)

# CUDA 소스 파일 (CUDA가 활성화된 경우에만)
set(CUDA_SOURCES "")
if(ENABLE_CUDA)
    set(CUDA_SOURCES src/Utility/cuda_processor.cu)
    set_source_files_properties(${CUDA_SOURCES} PROPERTIES LANGUAGE CUDA)
endif()

# add executable
add_executable(Luka
    src/main.cc
    src/SequenceStarter/sequence_starter.cc
    src/SequenceStarter/sequence_starter.h
    src/CommandProcessor/command_processor.cc
    src/Utility/converter.h
    ${DATA_PROCESSOR_SOURCES}
    ${COMMAND_PROCESSOR_SOURCES}
    ${CUDA_SOURCES}
)

# set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compile option
target_compile_options(Luka PRIVATE -Wall -Wextra -Werror)

# Compile commands (clangd, VSCode)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Link library
target_link_libraries(Luka PRIVATE OpenXLSX::OpenXLSX yaml-cpp)

# CUDA 라이브러리 링크 (CUDA가 활성화된 경우)
if(ENABLE_CUDA)
    target_link_libraries(Luka PRIVATE CUDA::cudart)
    set_target_properties(Luka PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif()

# include 
target_include_directories(Luka PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)
file(GLOB_RECURSE ALL_SOURCE_FILES
"${PROJECT_SOURCE_DIR}/src/*.cc"
"${PROJECT_SOURCE_DIR}/src/*.h"
)
add_custom_target(run_cpplint
    COMMAND cpplint --filter=-build/include_subdir ${ALL_SOURCE_FILES}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

