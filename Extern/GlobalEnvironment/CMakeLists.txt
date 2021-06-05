cmake_minimum_required(VERSION 3.13.1)

#Set the GlobalEnvironment version map here
set (GLOBALENVIRONMENT 1.0.0)

#TODO: Add util submodule

# Add a define so it exports the definitions
add_compile_definitions(DLL_EXPORTS)

# Define the library
add_library(GlobalEnvironment SHARED)

target_sources(
   GlobalEnvironment
   PRIVATE
      ${GLOBALENVIRONMENT}/GlobalEnvironment.cpp
      ${GLOBALENVIRONMENT}/GlobalEnvironment.h
)

target_include_directories(
   GlobalEnvironment
   INTERFACE
      ${GLOBALENVIRONMENT}
)