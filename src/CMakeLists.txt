add_executable(forall_constoprim.exe forall_constoprim.cpp)
  set_source_files_properties(forall_constoprim.cpp PROPERTIES LANGUAGE CUDA)
  target_link_libraries(forall_constoprim.exe PRIVATE
      Headers_Base 
      ${LIB_DEP} 
      nlohmann_json::nlohmann_json
  )
  target_include_directories(forall_constoprim.exe PRIVATE ${CMAKE_BINARY_DIR})