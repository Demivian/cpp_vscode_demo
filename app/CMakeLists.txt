add_executable(main
  main.cpp
)

target_link_libraries(main PRIVATE core)
target_compile_features(main PRIVATE cxx_std_14)
enable_project_warnings(main)
