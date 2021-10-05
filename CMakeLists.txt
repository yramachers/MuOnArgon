cmake_minimum_required(VERSION 3.12)
project(warwick-legend VERSION 0.1.0)

# Compiler/build settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CTest if we need it
include(CTest)

# Dependencies
find_package(Geant4 10.6 REQUIRED gdml ui_all vis_all)

# Build
add_executable(muonargon
  muonargon.cc
  src/MAActionInitialization.cc
  src/MABiasChangeCrossSection.cc
  src/MABiasMultiParticleChangeCrossSection.cc
  src/MACrystalHit.cc
  src/MACrystalSD.cc
  src/MADetectorConstruction.cc
  src/MAEventAction.cc
  src/MAPrimaryGeneratorAction.cc
  src/MARunAction.cc
  src/MAStackingAction.cc
  src/MATrackingAction.cc
  src/MATrajectory.cc)
target_include_directories(muonargon PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(muonargon PRIVATE ${Geant4_LIBRARIES})

# Copy macro needed to run in interactive mode to build directory.
# By default, the macro is assumed to be in the working directory
# where warwick-legend is run from.
configure_file(vis.mac vis.mac COPYONLY)

# Test
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
