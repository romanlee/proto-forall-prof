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


void dump_run_info(int nx, int N_ext){
  ordered_json run_info;

  // store run info in json obj
  run_info["input params"]["nx"] = nx;
  run_info["input params"]["N_ext"] = N_ext;

  run_info["Git describe"] = GIT_DESCRIBE;
  run_info["Git branch"] = GIT_BRANCH;

  run_info["CUDA compiler version"] = CMAKE_CUDA_COMPILER_VERSION;
  run_info["NVIDIA driver version"] = NVIDIA_DRIVER_VERSION;
  run_info["NVIDIA GPU name"] = NVIDIA_GPU_NAME;

  run_info["Build type"] = CMAKE_BUILD_TYPE;

  // dump to file
  std::ofstream file("run_info.json");
  if (file.is_open()) {
      file << run_info.dump(4) << std::endl; // 4 spaces for indentation
      file.close();
      std::cout << "run_info.json written successfully!" << std::endl;
  } else {
      std::cerr << "Error opening file for writing!" << std::endl;
  }
}


int main(int argc, char* argv[]){

  // Default input filename
  std::string input_filename = "input.json";
  
  // Check if input filename is provided as command line argument
  if (argc > 1) {
    input_filename = argv[1];
  }

  // read input params
  std::ifstream f(input_filename);
  
  // Check if file exists and can be opened
  if (!f.is_open()) {
    std::cerr << "Error: Could not open input file '" << input_filename << "'" << std::endl;
    std::cerr << "Please check that the file exists and is readable." << std::endl;
    return 1;
  }

  json input;
  try {
    input = json::parse(f);
  } catch (const json::parse_error& e) {
    std::cerr << "Error: Failed to parse JSON from file '" << input_filename << "'" << std::endl;
    std::cerr << "Parse error: " << e.what() << std::endl;
    f.close();
    return 1;
  }
  
  f.close();

// int main(){

//   // read input params
//   std::ifstream f("input.json");
//   json input = json::parse(f);

  int nx = input["nx"];
  int N_ext = input["N_ext"]; // Set >1 for "light kernel" test, else  set =1

  // printf("nx=%d, N_ext=%d, N_int=%d\n", nx, N_ext, N_int);

#ifdef GET_TIMINGS
  std::filesystem::path currentPath = std::filesystem::current_path();
  std::filesystem::path fullPath = currentPath / "TIMINGS.txt"; 
  PR_TIMER_SETFILE(fullPath.string());
#endif 

  // std::cout << "Default memory type: " << parseMemType(MEMTYPE_DEFAULT) << std::endl;

  // Set up the input array
  Stencil<double> S = Stencil<double>::Laplacian();

  Box rangeBox = Box::Cube(nx); //8, 16, 32, 64...

  BoxData<double, DIM+2> A(S.domain(rangeBox));
  BoxData<double, DIM+2> B(rangeBox);

  A.setVal(1);
  B.setVal(0);

  {
    PR_TIME("stenciltimer");

    for (int i=0; i<N_ext; i++){
      B |= S(A); //replace contents of B with S(A) in-place
    }

    // ensures that the timer only stops once the last kernel is done
    cudaDeviceSynchronize();
  }

#ifdef GET_TIMINGS
  // Screws up Nsight systems (except when...the PR_TIMER_SETFILE is in the same dir as the .nsys-rep???)
  PR_TIMER_REPORT();
#endif

  dump_run_info(nx, N_ext);
  
  std::cout << "Done!" << std::endl;

  return 0;
}
