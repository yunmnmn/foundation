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
   virtual void RegisterAllocator(Util::HashName p_hashName, AllocatorTracker* allocator) = 0;
   virtual void UnregisterAllocator(Util::HashName p_hashName) = 0;
};

}; // namespace Memory
}; // namespace Foundation
