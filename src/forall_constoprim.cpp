// modified from proto/tests/ForallTests.cpp
#include "Proto.H"
#include "version.h"
#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

using namespace Proto;

#define GET_TIMINGS 1


PROTO_KERNEL_START
void consToPrim_temp(Var<double,DIM+2>& W, 
                     const Var<double, DIM+2>& U,
                     double gamma, int N_int)
{
  // Heavy kernel loop
  for (int j=0; j<N_int; j++){
    double rho = U(0);
    double v, v2=0.;
    W(0) = rho;
    
    for (int i = 1; i <= DIM; i++)
      {
        v = U(i) / rho;
        
        W(i) = v;
        v2 += v*v;
      }
    
    W(DIM+1) = (U(DIM+1)-.5*rho*v2) * (gamma-1.);
  }
}
PROTO_KERNEL_END(consToPrim_temp, consToPrim)


void dump_run_info(int nx, int N_ext, int N_int){
  ordered_json run_info;

  run_info["input params"]["nx"] = nx;
  run_info["input params"]["N_ext"] = N_ext;
  run_info["input params"]["N_int"] = N_int;

  run_info["Git describe"] = GIT_DESCRIBE;
  run_info["Git branch"] = GIT_BRANCH;

  run_info["CUDA compiler version"] = CMAKE_CUDA_COMPILER_VERSION;
  run_info["NVIDIA driver version"] = NVIDIA_DRIVER_VERSION;
  run_info["NVIDIA GPU name"] = NVIDIA_GPU_NAME;

  run_info["Build type"] = CMAKE_BUILD_TYPE;

  std::ofstream file("run_info.json");
  if (file.is_open()) {
      file << run_info.dump(4) << std::endl; // 4 spaces for indentation
      file.close();
      std::cout << "run_info.json written successfully!" << std::endl;
  } else {
      std::cerr << "Error opening file for writing!" << std::endl;
  }
}


int main(){
  // int nx = 16;
  // int N_ext = 10;
  // int N_int = 1;

  // // read input params
  // std::ifstream f("input.json");
  // json input = json::parse(f);

  // int nx = input["nx"];
  // int N_ext = input["N_ext"]; // Set >1 for "light kernel" test, else  set =1
  // int N_int = input["N_int"]; // Set >1 for "heavy kernel" test, else set =1

#ifdef GET_TIMINGS
  std::filesystem::path currentPath = std::filesystem::current_path();
  std::filesystem::path fullPath = currentPath / "TIMINGS.txt"; 
  PR_TIMER_SETFILE(fullPath.string());
#endif 

  // std::cout << "Default memory type: " << parseMemType(MEMTYPE_DEFAULT) << std::endl;

  // Set up the input array
  Box srcBox = Box::Cube(nx);

  BoxData<double,DIM+2> U(srcBox,1);
  BoxData<double,DIM+2> W(srcBox,1);
  BoxData<double,DIM+2> V(srcBox,1);
  BoxData<short> X(srcBox);
  BoxData<double,DIM+2> Y(srcBox,1);

  const double gamma = 1.4;  

  {
    PR_TIME("forall_zone");

    // Light kernel loop
    for (int i=0; i<N_ext; i++){
      // forallInPlace<double,DIM+2,MEMTYPE_DEFAULT,1,1>(consToPrim,W,U,gamma,N_int);
      forallInPlace(consToPrim,W,U,gamma,N_int);
    }

    // ensures that the timer only stops once the last kernel is done
    cudaDeviceSynchronize();

  }

#ifdef GET_TIMINGS
  // Screws up Nsight systems (except when...the PR_TIMER_SETFILE is in the same dir as the .nsys-rep???)
  PR_TIMER_REPORT();
#endif

  dump_run_info(nx, N_ext, N_int);

  return 0;
}