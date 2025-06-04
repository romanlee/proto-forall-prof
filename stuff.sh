# configure
rm -r build/
cmake -B build -D DIM=2 -D ENABLE_CUDA=ON -D DEBUG=ON -D VERBOSE=0

# build
cmake --build build/ --target LightKernel -j 22

# profile
nsys profile -o roman/LightKernel/LightKernel%n ./build/bin/LightKernel
