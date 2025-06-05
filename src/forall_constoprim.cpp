// modified from tests/ForallTests.cpp
// #include <gtest/gtest.h>
#include "Proto.H"
#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace Proto;

#define NUMCOMPS DIM+2
#define GET_TIMINGS 1


PROTO_KERNEL_START
void f_threshold_temp( Var<short>& a_tags,
             Var<double, NUMCOMPS>& a_U)
{
  double thresh = 1.001;
  if (a_U(0) > thresh) {a_tags(0) = 1;}
  else {a_tags(0) = 0;};
};
PROTO_KERNEL_END(f_threshold_temp, f_threshold);

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

int main(){

  // read input params
  std::ifstream f("input.json");
  json input = json::parse(f);

  int nx = input["nx"];
  int N_ext = input["N_ext"]; // Set >1 for "light kernel" test, else  set =1
  int N_int = input["N_int"]; // Set >1 for "heavy kernel" test, else set =1

  // printf("nx=%d, N_ext=%d, N_int=%d\n", nx, N_ext, N_int);

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

  return 0;
}
