cmake_minimum_required(VERSION 3.13.1)

# Check if it's a root CMakeList or a added subdirectory
#if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeUtils")
#endif()

include(Utils)

project(
   Foundation
   VERSION 0.0.1
   LANGUAGES
      CXX
      C
)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Extern)

# Define the library
add_library(Foundation)

# Get the platform specific sources
PlatformSource(platformSource)
target_sources(
   Foundation 
   PRIVATE 
      Source/Container/ContainerTypes.cpp
      Source/FileSystem/FileSystem.cpp
      Source/Memory/BaseAllocator.cpp
      Source/Memory/BaseSchema.cpp
      Source/Memory/EastlAllocator.cpp
      Source/Memory/MemoryManager.cpp
      Source/Memory/StaticEaStlAllocator.cpp
      Source/Memory/TlsfSchema.cpp
      Source/Parallel/SpinLock.cpp
      Source/Util/Assert.cpp
      Source/Util/Atomic.cpp
      Source/Util/HashName.cpp
      Source/Util/Logger.cpp
      Source/Util/MurmurHash3.cpp
      # Include the platform specific files
      ${platformSource}
)

# Get the platform specific includes
PlatformPath(platformInclude)
target_include_directories(
   Foundation
   PRIVATE
      Source
   PUBLIC
      Include
      # Include the platform specific includes
      Include/${platformInclude}
)

set_target_properties(
   Foundation 
   PROPERTIES
      DEBUG_POSTFIX "d"
)

target_link_libraries(
   Foundation
   PUBLIC
      EASTL
   PRIVATE
      tlsf
)
