rm -r build; rm roman/*/TIMINGS.txt
# cmake -B build -D DIM=2 -D ENABLE_MPI=ON -D AMR=ON -D MMB=ON -D ENABLE_CUDA=ON -D ENABLE_HIP=OFF -D ENABLE_HDF5=ON -D OPS=ON -D DEBUG=ON -D VERBOSE=0 -D ENABLE_ALL_TESTS=ON -D DISABLE_INIT_CHECKS=0 -D ENABLE_ROMAN=ON
cmake -B build -D DIM=2 -D ENABLE_CUDA=ON -D DEBUG=ON -D VERBOSE=0 -D ENABLE_ROMAN=ON
cmake --build build/ --target LightKernel -j 22

nsys profile -o roman/LightKernel/LightKernel%n ./build/bin/LightKernel
