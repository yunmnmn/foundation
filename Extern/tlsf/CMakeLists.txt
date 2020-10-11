cmake_minimum_required(VERSION 3.13.1)

#Set the tlsf version map here
set (TLSF_VERSION 1.0.0)

# Define the library
add_library(tlsf)

target_sources(
   tlsf
   PRIVATE
      ${TLSF_VERSION}/tlsf.c
)

target_include_directories(
   tlsf
   INTERFACE
      ${TLSF_VERSION}
)