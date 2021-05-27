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

if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

add_subdirectory(Extern)

# Define the library
add_library(Foundation)

# Get the platform specific sources
PlatformSource(platformSource)
target_sources(
   Foundation 
   PRIVATE 
      #include the cpp
      Source/Memory/BaseAllocator.cpp
      Source/Memory/MemoryManager.cpp
      Source/Memory/BootstrapAllocator.cpp
      Source/Util/Assert.cpp
      Source/Util/Atomic.cpp
      Source/Util/HashName.cpp
      Source/Util/Logger.cpp
      Source/Util/MurmurHash3.cpp
      # Include the platform specific files
      ${platformSource}
      # Include the headers
      Include/Memory/BaseAllocator.h
      Include/Memory/BaseSchema.h
      Include/Memory/BootstrapAllocator.h
      Include/Memory/ClassAllocator.h
      Include/Memory/MemoryManager.h
      Include/Memory/MemoryManagerInterface.h
      Include/Memory/PoolSchema.h
      Include/Memory/StaticEastlAllocatorWrapper.h
      Include/Memory/EastlAllocatorWrapper.h
      Include/Memory/StaticAllocatorWrapper.h
      Include/Memory/StdAllocator.h
      Include/Memory/TlsfSchema.h
      Include/Util/Assert.h
      Include/Util/Atomic.h
      Include/Util/HashName.h
      Include/Util/Logger.h
      Include/Util/Macro.h
      Include/Util/ManagerInterface.h
      Include/Util/MurmurHash3.h
      Include/Util/Util.h
      Include/Std/string_bootstrap.h
      Include/Std/vector_bootstrap.h
      Include/Std/unordered_map_bootstrap.h
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

if(MSVC)
  target_compile_options(Foundation PRIVATE /W4 /WX)
endif()

set_target_properties(
   Foundation 
   PROPERTIES
      DEBUG_POSTFIX "d"
)

target_link_libraries(
   Foundation
   PUBLIC
      EASTL
      tlsf
      GlobalEnvironment
   PRIVATE
)