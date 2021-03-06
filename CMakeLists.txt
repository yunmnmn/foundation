cmake_minimum_required(VERSION 3.13.1)

# Check if it's a root CMakeList or a added subdirectory
#if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeUtils")
#endif()

include(CMakeUtils/Utils.cmake)

project(
   Foundation
   VERSION 0.0.1
   LANGUAGES
      CXX
      C
)

# Enable the latest C++ features
if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

# Add the subdirectory of submodules
add_subdirectory(Extern)

# Define the library
add_library(Foundation)

# Get the platform specific sources
PlatformSource(_platformSource)
target_sources(
   Foundation 
   PRIVATE 
      #include the cpp
      Source/Foundation.cpp
      Source/Memory/BaseAllocator.cpp
      Source/Memory/MemoryManager.cpp
      Source/Memory/BootstrapAllocator.cpp
      Source/Util/Assert.cpp
      Source/Util/Atomic.cpp
      Source/Util/HashName.cpp
      Source/Util/Logger.cpp
      Source/Util/MurmurHash3.cpp
      # Include the platform specific files
      ${_platformSource}
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
      Include/IO/FileIOInterface.h
      Include/IO/FileWatcherInterface.h
)

GenerateFolderStructure(Foundation)

# Get the platform specific includes
PlatformPath(platformInclude)
target_include_directories(
   Foundation
   PRIVATE
      Source
      # Include the platform specific includes
      Source/${platformInclude}
      Source/_Platform/Common
   PUBLIC
      Include
      # Include the platform specific includes
      Include/${platformInclude}
      Include/_Platform/Common
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

