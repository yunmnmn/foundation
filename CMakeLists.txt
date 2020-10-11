cmake_minimum_required(VERSION 3.5.1)

project(
   Foundation
   VERSION 0.0.1
   LANGUAGES
      CXX
      C
)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Extern)

# Append the modules of the third party libraries of this
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/ThirdParty

# Define the library
add_library(Foundation)

#TODO: create a helper function that adds the platform specific files
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
)

#TODO: create a helper f unction that includes the platform specific files
target_include_directories(
   Foundation
   PRIVATE
      Source
   PUBLIC
      Include
)

set_target_properties(
   Foundation 
   PROPERTIES
      DEBUG_POSTFIX "d"
)

target_link_libraries(
   Foundation
   PUBLIC
      #EASTL
   PRIVATE
      tlsf
)
