# cmake_minimum_required(VERSION 3.21)  # uses enable_language(HIP)
cmake_minimum_required(VERSION 3.20)  # uses enable_language(HIP)

project(Proto_forall_prof VERSION 1.0 DESCRIPTION "Proto forall profiling") # LANGUAGES CXX) # HDF5 fails with LANGUAGES argument

enable_language(CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # if OFF, compilation will revert to older standard if C++17 unavailable
set(CMAKE_CXX_EXTENSIONS OFF)  # if OFF, compilation flag uses c++17, not gnu++17
set(CMAKE_VERBOSE_MAKEFILE ON) # if ON, prints the full compilation command when building


# --------------------------------------------------------------------------------------
# Json
# --------------------------------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)


# --------------------------------------------------------------------------------------
# Proto mess
# Surely there's a better way to do this
# --------------------------------------------------------------------------------------
set(CMAKE_POSITION_INDEPENEDENT_CODE TRUE)

# # silence all compiler warnings
# add_compile_options(-w)

# Proto option variables
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(AMR "Build AMR libraries" OFF)
option(MMB "Build MMB libraries" OFF)
option(OPS "Build Operator libraries" OFF)
option(BLIS "Use BLIS instead of BLAS" OFF)
option(DEBUG "Building with DEBUG ON" ON)
option(ENABLE_MPI "Build using MPI" ON)
option(ENABLE_OPENMP "Build using OpenMP" OFF)
option(ENABLE_CUDA "Build using CUDA" OFF)
option(ENABLE_HIP "Build using HIP" OFF)
option(ENABLE_HDF5 "Build using HDF5" ON)
option(ENABLE_TESTS "Build default tests only" ON)
option(ENABLE_ALL_TESTS "Build all tests" OFF)
option(ENABLE_EXAMPLES "Build examples" ON)
option(ENABLE_DOCS "Build Proto documentation" OFF)
option(MEMCHECK "turns on code in BoxData that checks that copying/aliasing is working correctly" OFF)
option(MEMTRACK "print the amount of data allocated per protoMalloc" OFF)
option(TIMERS "whether to turn on timers" ON)
option(DISABLE_INIT_CHECKS "Disables automatic checks for uninitialized/NAN/inf values in BoxData" OFF)

# Proto cache variables
set(VERBOSE 0 CACHE STRING "Verbosity of output")
set(DIM 3 CACHE STRING "Dimensionality of examples")
set(STACK 4294967296 CACHE STRING "Stack size")

message(STATUS "Dimensionality: ${DIM}")
message(STATUS "Stack size: ${STACK}")
message(STATUS "Verbosity: ${VERBOSE}")

add_compile_definitions(PR_VERBOSE=${VERBOSE})
add_compile_definitions(DIM=${DIM})
add_compile_definitions(PR_STACK_ALLOC=${STACK})

if (NOT CMAKE_BUILD_TYPE)
    if (DEBUG)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose build type, \
        options are: Debug Release RelWithDebInfo" FORCE)
    else()
        add_compile_definitions(NDEBUG)
        add_compile_definitions(PR_DISABLE_INIT_CHECKS)
        set(VERBOSE 0 CACHE STRING "Verbosity of output")
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose build type, \
        options are: Debug Release RelWithDebInfo" FORCE)
    endif()
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (DISABLE_INIT_CHECKS)
    message(STATUS "Data initialization checks are DISABLED")
    add_compile_definitions(PR_DISABLE_INIT_CHECKS)
endif()

if (AMR)
    message(STATUS "AMR libraries are enabled")
    add_compile_definitions(PR_AMR)
endif()

if (MMB)
    message(STATUS "MMB libraries are enabled")
    add_compile_definitions(PR_MMB)
endif()

if(MEMCHECK)
    message(STATUS "MEMCHECK is enabled")
    add_compile_definitions(PROTO_MEM_CHECK)
endif()

if(MEMTRACK)
    message(STATUS "MEMTRACK is enabled")
    add_compile_definitions(memoryFootPrint)
endif()

if(NOT TIMERS)
    message(STATUS "Proto Timers are disabled")
    add_compile_definitions(PR_TURN_OFF_TIMERS)
endif()

if (ENABLE_HIP)
    set(ENV{CXX} "hipcc")  # should set CMAKE_CXX_COMPILER
endif()

# setup packages
if(BLIS)
  set(OPS ON) # doesn't make sense to use blis without Ops
endif()

if(OPS)
    message(STATUS "Operator libraries are enabled")
    add_compile_definitions(PR_OPS)
    if (DEFINED ENV{OLCF_NETLIB_LAPACK_ROOT})
        set(LAPACK_DIR $ENV{OLCF_NETLIB_LAPACK_ROOT})
    elseif(DEFINED ENV{MKLROOT})
        set(LAPACK_DIR $ENV{MKLROOT})
        add_compile_definitions(MKL)
    elseif(DEFINED ENV{LAPACK_DIR})  # must do on MacOS after `brew install lapack` to get lapacke.h
        set(LAPACK_DIR $ENV{LAPACK_DIR})
    endif()
    find_package(LAPACK REQUIRED)
    if(NOT BLIS)
        find_package(BLAS REQUIRED)
    elseif(NOT EXISTS ${CMAKE_SOURCE_DIR}/blis/configure)
        submodule_error("BLIS")
    elseif(NOT EXISTS ${CMAKE_SOURCE_DIR}/blis/include)
        list(APPEND BLIS_CXX "gcc" "icc" "clang")
        list(FIND BLIS_CXX $ENV{CC} IDX)
        if(NOT DEFINED ENV{CC} OR ${IDX} EQUAL -1)
          set(ENV{CC} "gcc")
          set(ENV{CXX} "g++")
        endif()
        execute_process(COMMAND ${CMAKE_SOURCE_DIR}/blis/configure auto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/blis)
    endif()
endif()

if(ENABLE_MPI)
    add_compile_definitions(PR_MPI)
    list(APPEND LIB_DEP mpi)
    set(ENABLE_WRAP_ALL_TESTS_WITH_MPIEXEC TRUE)  # needed by BLT to make mpiexec commands
    if(DEFINED CMAKE_APPLE_SILICON_PROCESSOR)
        get_filename_component(MPI_LINK /usr/local/bin/mpiexec REALPATH)
        get_filename_component(MPI_PATH ${MPI_LINK} DIRECTORY)
        set(MPIEXEC_EXECUTABLE ${MPI_PATH}/mpiexec CACHE FILEPATH "full path for mpiexec")
    endif()
	find_package(MPI REQUIRED)
endif()

if(ENABLE_CUDA OR ENABLE_HIP)
    add_compile_definitions(PROTO_ACCEL)
    if(ENABLE_CUDA AND ENABLE_HIP)
        message(FATAL_ERROR "Must specify at most one of CUDA and HIP")
    endif()
    if(ENABLE_HIP)
        add_compile_definitions(PROTO_HIP)
        enable_language(HIP)
        list(APPEND LIB_DEP blt::hip)
    else()
        add_compile_definitions(PROTO_CUDA)
        if(CMAKE_BUILD_TYPE STREQUAL Debug)
            set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -G")
            add_compile_definitions(PROTO_DEBUG)
        endif()
        if(DEFINED CUDA_TOOLKIT_ROOT_DIR)
            set(CMAKE_CUDA_COMPILER "${CUDA_TOOLKIT_ROOT_DIR}/bin/nvcc" CACHE PATH "")
        else()
            set(ENV{CUDACXX} nvcc)
        endif()
        set(CMAKE_CUDA_HOST_COMPILER ${CMAKE_CXX_COMPILER} CACHE PATH "Setting for BLT")
        enable_language(CUDA)
        set(CMAKE_CUDA_SEPARABLE_COMPILATION ON CACHE BOOL "")
        if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
            set(CMAKE_CUDA_ARCHITECTURES 70 CACHE STRING "Compute architecture needed by NVCC")
        endif()
        if(CMAKE_CUDA_COMPILER_VERSION VERSION_LESS 10)
            message(FATAL_ERROR "CUDA version >=10 needed, found ${CMAKE_CUDA_COMPILER_VERSION}")
        elseif(CMAKE_CUDA_COMPILER_VERSION VERSION_EQUAL 11)
            if(CMAKE_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 9)
                message(FATAL_ERROR "GCC version 9 needed, found ${CMAKE_CXX_COMPILER_VERSION}")
            endif()
        endif()
        set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --expt-relaxed-constexpr --expt-extended-lambda")
        list(APPEND LIB_DEP cuda)

        # -------------
        # WIP
        # -------------
        message(STATUS "CUDA Compiler: ${CMAKE_CUDA_COMPILER}")
        message(STATUS "CUDA Compiler Version: ${CMAKE_CUDA_COMPILER_VERSION}")
        message(STATUS "CUDA Compiler ID: ${CMAKE_CUDA_COMPILER_ID}")

        # Try to get driver info
        find_program(NVIDIA_SMI nvidia-smi)
        if(NVIDIA_SMI)
            execute_process(
                COMMAND ${NVIDIA_SMI} --query-gpu=driver_version,name --format=csv,noheader,nounits
                OUTPUT_VARIABLE NVIDIA_INFO
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET
            )
            if(NVIDIA_INFO)
                message(STATUS "NVIDIA Driver & GPU: ${NVIDIA_INFO}")
            endif()
        endif()
        # -------------


    endif()
endif()

if(ENABLE_OPENMP)
    list(APPEND LIB_DEP blt::openmp)
endif()

if(ENABLE_HDF5)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
      set(FLAG ${CMAKE_CXX_FLAGS_RELEASE})
      string(FIND ${FLAG} "-DNDEBUG" INDEX)
      if(NOT INDEX EQUAL -1)
        string(SUBSTRING ${FLAG} 0 4 FLAG)
        unset(CMAKE_CXX_FLAGS_RELEASE CACHE)
        set(CMAKE_CXX_FLAGS_RELEASE ${FLAG} CACHE STRING "removed -DNDEBUG")
      endif()
    endif()
    add_compile_definitions(PR_HDF5)
    if(ENABLE_MPI)
        set(HDF5_PREFER_PARALLEL TRUE)
		if(EXISTS $ENV{HDF5_DIR})
			string(FIND $ENV{HDF5_DIR} "parallel" IDX)
			string(FIND $ENV{HDF5_DIR} "mpi" XDI)
			if(${IDX} EQUAL -1 AND ${XDI} EQUAL -1)
				message(WARNING "HDF5 library in path might not be parallel for MPI build")
			endif()
		elseif(EXISTS $ENV{HDF5_ROOT})
			string(FIND $ENV{HDF5_ROOT} "parallel" IDX)
			string(FIND $ENV{HDF5_ROOT} "mpi" XDI)
			if(${IDX} EQUAL -1 AND ${XDI} EQUAL -1)
				message(WARNING "HDF5 library in path might not be parallel for MPI build")
			endif()
		endif()
    endif()
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        get_filename_component(HDF5_LINK /usr/local/include/hdf5.h REALPATH)
        string(REGEX MATCH [0-9]+\\.[0-9]+\\.[0-9]+_?[0-9]* HDF5_VERSION ${HDF5_LINK})
        if(NOT DEFINED ENV{HDF5_ROOT})
            if(ENABLE_MPI)
                set(HDF5_ROOT /usr/local/Cellar/hdf5-mpi/${HDF5_VERSION} CACHE PATH "parallel HDF5 directory")
            elseif(DEFINED CMAKE_APPLE_SILICON_PROCESSOR)
                set(HDF5_ROOT /usr/local/hdf5/${HDF5_VERSION} CACHE PATH "serial HDF5 directory")
            else()
                set(HDF5_ROOT /usr/local/Cellar/hdf5/${HDF5_VERSION} CACHE PATH "serial HDF5 directory")
            endif()
            if(NOT IS_DIRECTORY HDF5_ROOT AND NOT IS_DIRECTORY ${HDF5_ROOT})
                message(FATAL_ERROR "HDF5 is installed but not in /usr/local/Cellar or stored in $HDF5_ROOT")
            endif()
        endif()
        find_package(HDF5 REQUIRED)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        if(NOT DEFINED ENV{HDF5_ROOT} AND NOT DEFINED ENV{OLCF_HDF5_ROOT} AND NOT DEFINED ENV{CRAY_HDF5_DIR})
            if(ENABLE_MPI)
                set(HDF5_INC /usr/include/hdf5/openmpi FILEPATH)
                set(HDF5_LIB /usr/lib/x86_64-linux-gnu/hdf5/openmpi FILEPATH)
            else()
                set(HDF5_INC /usr/include/hdf5/serial FILEPATH)
                set(HDF5_LIB /usr/lib/x86_64-linux-gnu/hdf5/serial FILEPATH)
            endif()
        else()
            find_package(HDF5 REQUIRED)
        endif()
    endif()
endif()


# -----------------
# add_subdirectory(include)
# from proto/include/CMakeLists.txt
# -----------------
add_library(Headers_Base INTERFACE)

target_include_directories(Headers_Base INTERFACE
    /global/homes/r/romanlee/Documents/codes/proto/include/
    /global/homes/r/romanlee/Documents/codes/proto/include/base/
)

# target_sources(Headers_Base INTERFACE Proto.H) # not necessary? Not sure where I got this line
target_link_libraries(Headers_Base INTERFACE nvToolsExt)


# blt_add_library(NAME Headers_Base HEADERS Proto.H
#                 INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}
#                 ${CMAKE_CURRENT_SOURCE_DIR}/base)
# # blt_add_library(NAME Headers_AMR HEADERS ProtoAMR.H
# #                 INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}
# #                 ${CMAKE_CURRENT_SOURCE_DIR}/amr
# #                 DEPENDS_ON Headers_Base)
# # blt_add_library(NAME Headers_MMB HEADERS ProtoMMB.H
# #                 INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}
# #                 ${CMAKE_CURRENT_SOURCE_DIR}/mmb
# #                 DEPENDS_ON Headers_Base)
# # blt_add_library(NAME Headers_OPS HEADERS ProtoOps.H
# #                 INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}
# #                 ${CMAKE_CURRENT_SOURCE_DIR}/ops
# #                 DEPENDS_ON Headers_Base LAPACK::LAPACK)
# if(BLIS)
#     message(STATUS "Linking BLIS library")
#     target_compile_definitions(Headers_OPS INTERFACE PR_BLIS)
#     file(GLOB CONFIG RELATIVE ${CMAKE_SOURCE_DIR}/blis/include/ ${CMAKE_SOURCE_DIR}/blis/include/*)
#     if(NOT EXISTS ${CMAKE_SOURCE_DIR}/blis/include/${CONFIG}/blis.h)
#       execute_process(COMMAND make -j WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/blis)
#     endif()
#     blt_import_library(NAME blis INCLUDES ${CMAKE_SOURCE_DIR}/blis/include/${CONFIG}
#     TREAT_INCLUDES_AS_SYSTEM ON LIBRARIES ${CMAKE_SOURCE_DIR}/blis/lib/${CONFIG}/libblis.a)
#     blt_patch_target(NAME Headers_OPS DEPENDS_ON blis)
# endif()
# if(OPS)
#     blt_patch_target(NAME Headers_Base DEPENDS_ON Headers_OPS)
# endif()
# if(DEFINED ENV{OLCF_OPENBLAS_ROOT})
#   target_link_directories(Headers_OPS INTERFACE $ENV{OLCF_OPENBLAS_ROOT}/lib/)
#   target_link_libraries(Headers_OPS INTERFACE openblas)
# endif()
# if(ENABLE_CUDA)
#     target_link_libraries(Headers_Base INTERFACE nvToolsExt)
#     target_link_libraries(Headers_AMR INTERFACE nvToolsExt)
#     target_link_libraries(Headers_MMB INTERFACE nvToolsExt)
#     target_link_libraries(Headers_OPS INTERFACE nvToolsExt)
# endif()
# if(ENABLE_HIP)
#     target_include_directories(Headers_Base INTERFACE $ENV{ROCM_PATH}/roctracer/include/)
#     target_include_directories(Headers_AMR  INTERFACE $ENV{ROCM_PATH}/roctracer/include/)
#     target_include_directories(Headers_MMB  INTERFACE $ENV{ROCM_PATH}/roctracer/include/)
#     target_include_directories(Headers_OPS  INTERFACE $ENV{ROCM_PATH}/roctracer/include/)
#     target_include_directories(Headers_Base INTERFACE $ENV{HIP_PATH}/include/)
#     target_link_directories(Headers_Base INTERFACE $ENV{ROCM_PATH}/lib)
#     target_link_directories(Headers_AMR  INTERFACE $ENV{ROCM_PATH}/lib)
#     target_link_directories(Headers_MMB  INTERFACE $ENV{ROCM_PATH}/lib)
#     target_link_directories(Headers_OPS  INTERFACE $ENV{ROCM_PATH}/lib)
#     target_link_libraries(Headers_Base INTERFACE roctx64)
#     target_link_libraries(Headers_AMR INTERFACE roctx64)
#     target_link_libraries(Headers_MMB INTERFACE roctx64)
#     target_link_libraries(Headers_OPS INTERFACE roctx64)
# endif()
# if(ENABLE_MPI)
#     target_link_libraries(Headers_Base INTERFACE MPI::MPI_CXX)
#     target_link_libraries(Headers_AMR INTERFACE MPI::MPI_CXX)
#     target_link_libraries(Headers_MMB INTERFACE MPI::MPI_CXX)
#     target_link_libraries(Headers_OPS INTERFACE MPI::MPI_CXX)
#     target_include_directories(Headers_Base INTERFACE $ENV{MPICH_DIR}/include/)
#     target_include_directories(Headers_AMR INTERFACE $ENV{MPICH_DIR}/include/)
#     target_include_directories(Headers_MMB INTERFACE $ENV{MPICH_DIR}/include/)
#     target_include_directories(Headers_OPS INTERFACE $ENV{MPICH_DIR}/include/)
#     target_link_directories(Headers_Base INTERFACE $ENV{MPICH_DIR}/lib/)
#     target_link_directories(Headers_AMR INTERFACE $ENV{MPICH_DIR}/lib/)
#     target_link_directories(Headers_MMB INTERFACE $ENV{MPICH_DIR}/lib/)
#     target_link_directories(Headers_OPS INTERFACE $ENV{MPICH_DIR}/lib/)
#     target_link_libraries(Headers_Base INTERFACE mpi)
#     target_link_libraries(Headers_AMR INTERFACE mpi)
#     target_link_libraries(Headers_MMB INTERFACE mpi)
#     target_link_libraries(Headers_OPS INTERFACE mpi)
# endif()
# if(ENABLE_OPENMP)
#     target_link_libraries(Headers_Base INTERFACE OpenMP::OpenMP_CXX)
#     target_link_libraries(Headers_AMR  INTERFACE OpenMP::OpenMP_CXX)
#     target_link_libraries(Headers_MMB  INTERFACE OpenMP::OpenMP_CXX)
#     target_link_libraries(Headers_OPS  INTERFACE OpenMP::OpenMP_CXX)
# endif()
# if(DEFINED HDF5_INC)
#   target_include_directories(Headers_Base INTERFACE ${HDF5_INC}/include)
#   target_link_directories(Headers_Base INTERFACE ${HDF5_LIB}/lib)
#   target_link_libraries(Headers_Base INTERFACE hdf5)
# elseif(ENABLE_HDF5)
#   target_link_libraries(Headers_Base INTERFACE HDF5::HDF5)
# endif()

# if(ENABLE_EXAMPLES)
#     add_subdirectory(examples)
# endif()

# if(ENABLE_TESTS)
#     add_subdirectory(tests)
# endif()

# if(ENABLE_DOCUMENTATION)
#     add_subdirectory(docs)
# endif()

# ----------------------


# --------------------------------------------------------------------------------------
# --------------------------------------------------------------------------------------
find_package(Git QUIET)

if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --dirty --always
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_DESCRIBE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    set(GIT_BRANCH "unknown")
    set(GIT_DESCRIBE "unknown")
endif()

# Configure a header file
configure_file(
    "${CMAKE_SOURCE_DIR}/version.h.in"
    "${CMAKE_BINARY_DIR}/version.h"
    @ONLY
)


# --------------------------------------------------------------------------------------
# --------------------------------------------------------------------------------------
add_subdirectory(src)
