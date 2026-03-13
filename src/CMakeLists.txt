add_library(core
  map3d.cpp
)

target_include_directories(core PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(core PUBLIC cxx_std_14)
enable_project_warnings(core)

# Example for splitting src/ into submodules later:
#
# src/
#   CMakeLists.txt
#   field/
#     CMakeLists.txt
#     map3d.cpp
#   math/
#     CMakeLists.txt
#     matrix.cpp
#
# Then keep this file as the top-level owner of the core target:
#
# add_library(core)
# target_sources(core PRIVATE
#   common.cpp
# )
# add_subdirectory(field)
# add_subdirectory(math)
#
# And let each subdirectory append its own sources:
#
# field/CMakeLists.txt
#   target_sources(core PRIVATE
#     map3d.cpp
#   )
#
# math/CMakeLists.txt
#   target_sources(core PRIVATE
#     matrix.cpp
#   )
