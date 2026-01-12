cmake_minimum_required(VERSION 3.26)
project(Luka LANGUAGES CXX)

# Cuda support option
option(ENABLE_CUDA "Enable CUDA support" OFF)

# Auto-detect: Enable CUDA if both NVIDIA GPU (`nvidia-smi`) and CUDA Toolkit are present
if(NOT ENABLE_CUDA)
  find_program(NVIDIA_SMI_EXECUTABLE nvidia-smi)
  if(NVIDIA_SMI_EXECUTABLE)
    execute_process(COMMAND ${NVIDIA_SMI_EXECUTABLE} -L
      RESULT_VARIABLE NVIDIA_SMI_RESULT
      OUTPUT_VARIABLE NVIDIA_SMI_OUTPUT
      ERROR_QUIET)
    # nvidia-smi -L returns 0 if an NVIDIA GPU is present
    if(NOT NVIDIA_SMI_RESULT)
      find_package(CUDAToolkit QUIET)
      if(CUDAToolkit_FOUND)
        message(STATUS "NVIDIA GPU and CUDA Toolkit detected — enabling CUDA")
        set(ENABLE_CUDA ON CACHE BOOL "Enable CUDA support" FORCE)
      else()
        message(WARNING "NVIDIA GPU detected (nvidia-smi) but CUDA Toolkit not found. Install CUDA or set -DENABLE_CUDA=ON to force it.")
      endif()
    endif()
  endif()
endif()

if(ENABLE_CUDA)
    # Ensure CUDA compiler exists (nvcc) to avoid CMake errors during enable_language
    find_program(NVCC_EXECUTABLE nvcc)
    if(NOT NVCC_EXECUTABLE)
      message(WARNING "nvcc (CUDA compiler) not found in PATH; disabling CUDA support to avoid configuration errors. Install CUDA or set -DENABLE_CUDA=ON after installing CUDA.")
      set(ENABLE_CUDA OFF CACHE BOOL "Enable CUDA support" FORCE)
    else()
      # Ensure CMake has a CUDA architecture set (AUTO picks a suitable default)
      if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES OR CMAKE_CUDA_ARCHITECTURES STREQUAL "")
        # Use 'native' so CMake picks an appropriate default supported by the compiler
        set(CMAKE_CUDA_ARCHITECTURES native CACHE STRING "CUDA architectures" FORCE)
      endif()

      enable_language(CUDA)
      find_package(CUDAToolkit REQUIRED)
      message(STATUS "CUDA support enabled")
      add_compile_definitions(CUDA_ENABLED)
    endif()
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
file(GLOB ENVIRONMENTS_SOURCES
src/Environments/*.cc
src/Environments/*.h
)
file(GLOB CALCULATION_SOURCES
src/Calculation/*.cc
src/Calculation/*.h
)

# CUDA source files(if only cuda exists)
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
    ${ENVIRONMENTS_SOURCES}
    ${CALCULATION_SOURCES}
    ${CUDA_SOURCES}
)

# set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compile option
# Apply warnings for C++ and forward them appropriately for CUDA (use -Xcompiler for nvcc)
# For C++ sources, pass flags directly; for CUDA, forward to host compiler via -Xcompiler
target_compile_options(Luka PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Werror>
  $<$<COMPILE_LANGUAGE:CUDA>:-Xcompiler=-Wall -Xcompiler=-Wextra -Xcompiler=-Werror>
)

# Compile commands (clangd, VSCode)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Link library
target_link_libraries(Luka PRIVATE OpenXLSX::OpenXLSX yaml-cpp)
    
# CUDA library link (if CUDA is enabled)
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

