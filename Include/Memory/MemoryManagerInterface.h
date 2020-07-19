#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Util/Assert.h>
#include <Util/ManagerInterface.h>

namespace Foundation
{
namespace Memory
{
class AllocatorBase;

class MemoryManagerInterface : public Util::ManagerInterface<MemoryManagerInterface>
{
 public:
   virtual void RegisterAllocator(AllocatorBase* allocator) = 0;
   virtual void UnregisterAllocator(AllocatorBase* allocator) = 0;
};

}; // namespace Memory
}; // namespace Foundation
