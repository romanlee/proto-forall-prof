set(CMAKE_CUDA_COMPILER "/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/bin/nvcc")
set(CMAKE_CUDA_HOST_COMPILER "/opt/cray/pe/craype/2.7.32/bin/CC")
set(CMAKE_CUDA_HOST_LINK_LAUNCHER "/opt/cray/pe/craype/2.7.32/bin/CC")
set(CMAKE_CUDA_COMPILER_ID "NVIDIA")
set(CMAKE_CUDA_COMPILER_VERSION "12.4.131")
set(CMAKE_CUDA_DEVICE_LINKER "/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/bin/nvlink")
set(CMAKE_CUDA_FATBINARY "/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/bin/fatbinary")
set(CMAKE_CUDA_STANDARD_COMPUTED_DEFAULT "17")
set(CMAKE_CUDA_COMPILE_FEATURES "cuda_std_03;cuda_std_11;cuda_std_14;cuda_std_17")
set(CMAKE_CUDA03_COMPILE_FEATURES "cuda_std_03")
set(CMAKE_CUDA11_COMPILE_FEATURES "cuda_std_11")
set(CMAKE_CUDA14_COMPILE_FEATURES "cuda_std_14")
set(CMAKE_CUDA17_COMPILE_FEATURES "cuda_std_17")
set(CMAKE_CUDA20_COMPILE_FEATURES "")
set(CMAKE_CUDA23_COMPILE_FEATURES "")

set(CMAKE_CUDA_PLATFORM_ID "Linux")
set(CMAKE_CUDA_SIMULATE_ID "GNU")
set(CMAKE_CUDA_COMPILER_FRONTEND_VARIANT "")
set(CMAKE_CUDA_SIMULATE_VERSION "13.2")



set(CMAKE_CUDA_COMPILER_ENV_VAR "CUDACXX")
set(CMAKE_CUDA_HOST_COMPILER_ENV_VAR "CUDAHOSTCXX")

set(CMAKE_CUDA_COMPILER_LOADED 1)
set(CMAKE_CUDA_COMPILER_ID_RUN 1)
set(CMAKE_CUDA_SOURCE_FILE_EXTENSIONS cu)
set(CMAKE_CUDA_LINKER_PREFERENCE 15)
set(CMAKE_CUDA_LINKER_PREFERENCE_PROPAGATES 1)

set(CMAKE_CUDA_SIZEOF_DATA_PTR "8")
set(CMAKE_CUDA_COMPILER_ABI "ELF")
set(CMAKE_CUDA_BYTE_ORDER "LITTLE_ENDIAN")
set(CMAKE_CUDA_LIBRARY_ARCHITECTURE "")

if(CMAKE_CUDA_SIZEOF_DATA_PTR)
  set(CMAKE_SIZEOF_VOID_P "${CMAKE_CUDA_SIZEOF_DATA_PTR}")
endif()

if(CMAKE_CUDA_COMPILER_ABI)
  set(CMAKE_INTERNAL_PLATFORM_ABI "${CMAKE_CUDA_COMPILER_ABI}")
endif()

if(CMAKE_CUDA_LIBRARY_ARCHITECTURE)
  set(CMAKE_LIBRARY_ARCHITECTURE "")
endif()

set(CMAKE_CUDA_COMPILER_TOOLKIT_ROOT "/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4")
set(CMAKE_CUDA_COMPILER_TOOLKIT_LIBRARY_ROOT "/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4")
set(CMAKE_CUDA_COMPILER_LIBRARY_ROOT "/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4")

set(CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES "/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/targets/x86_64-linux/include")

set(CMAKE_CUDA_HOST_IMPLICIT_LINK_LIBRARIES "")
set(CMAKE_CUDA_HOST_IMPLICIT_LINK_DIRECTORIES "/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/targets/x86_64-linux/lib/stubs;/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/targets/x86_64-linux/lib")
set(CMAKE_CUDA_HOST_IMPLICIT_LINK_FRAMEWORK_DIRECTORIES "")

set(CMAKE_CUDA_IMPLICIT_INCLUDE_DIRECTORIES "/opt/cray/pe/mpich/8.1.30/ofi/gnu/12.3/include;/opt/cray/pe/libsci/24.07.0/GNU/12.3/x86_64/include;/opt/cray/pe/hdf5-parallel/1.14.3.1/gnu/12.3/include;/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/nvvm/include;/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/extras/CUPTI/include;/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/extras/Debugger/include;/opt/cray/pe/dsmml/0.3.0/dsmml/include;/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/math_libs/12.4/include;/usr/include/c++/13;/usr/include/c++/13/x86_64-suse-linux;/usr/include/c++/13/backward;/usr/lib64/gcc/x86_64-suse-linux/13/include;/usr/local/include;/usr/lib64/gcc/x86_64-suse-linux/13/include-fixed;/usr/x86_64-suse-linux/include;/usr/include")
set(CMAKE_CUDA_IMPLICIT_LINK_LIBRARIES "hdf5_hl_parallel;hdf5_parallel;darshan;lustreapi;z;cupti;cuda;sci_gnu_mpi;mpi_gnu_123;mpi_gtl_cuda;sci_gnu;dsmml;xpmem;gfortran;quadmath;mvec;m;stdc++;m;gcc_s;gcc;c;gcc_s;gcc")
set(CMAKE_CUDA_IMPLICIT_LINK_DIRECTORIES "/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/targets/x86_64-linux/lib/stubs;/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/targets/x86_64-linux/lib;/opt/cray/pe/mpich/8.1.30/ofi/gnu/12.3/lib;/opt/cray/pe/mpich/8.1.30/gtl/lib;/opt/cray/pe/libsci/24.07.0/GNU/12.3/x86_64/lib;/opt/cray/pe/hdf5-parallel/1.14.3.1/gnu/12.3/lib;/global/common/software/nersc9/darshan/3.4.6-gcc-13.2.1/lib;/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/lib64/stubs;/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/lib64;/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/nvvm/lib64;/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/extras/CUPTI/lib64;/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/cuda/12.4/extras/Debugger/lib64;/opt/nvidia/hpc_sdk/Linux_x86_64/24.5/math_libs/12.4/lib64;/opt/cray/pe/dsmml/0.3.0/dsmml/lib;/usr/lib64/gcc/x86_64-suse-linux/13;/usr/lib64;/lib64;/usr/x86_64-suse-linux/lib")
set(CMAKE_CUDA_IMPLICIT_LINK_FRAMEWORK_DIRECTORIES "")

set(CMAKE_CUDA_RUNTIME_LIBRARY_DEFAULT "STATIC")

set(CMAKE_LINKER "/usr/bin/ld")
set(CMAKE_AR "/usr/bin/ar")
set(CMAKE_MT "")
