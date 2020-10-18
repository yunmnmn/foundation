#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Util/ManagerInterface.h>
#include <Util/HashName.h>

namespace Foundation
{
namespace Memory
{
class AllocatorTracker;

class MemoryManagerInterface : public Util::ManagerInterface<MemoryManagerInterface>
{
 public:
   virtual void RegisterAllocator(HashName p_hashName, AllocatorTracker* allocator) = 0;
   virtual void UnregisterAllocator(HashName p_hashName) = 0;
};

}; // namespace Memory
}; // namespace Foundation
