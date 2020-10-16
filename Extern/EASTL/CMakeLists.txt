cmake_minimum_required(VERSION 3.13.1)

set(version 3.17.00)

add_subdirectory(${version})

# Additionally also add the headers, for visual assist
file(GLOB EASTL_HEADERS "${version}/include/EASTL/*.h")
target_sources(
   EASTL
   PRIVATE
      ${EASTL_HEADERS}
)