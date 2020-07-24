#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Util/Assert.h>
#include <Util/ManagerInterface.h>
#include <Util/HashName.h>

namespace Foundation
{
namespace Memory
{
class AllocatorInterface;

class MemoryManagerInterface : public Util::ManagerInterface<MemoryManagerInterface>
{
 public:
   virtual void RegisterAllocator(HashName p_hashName, BaseAllocator* allocator) = 0;
   virtual void UnregisterAllocator(HashName p_hashName) = 0;
};

}; // namespace Memory
}; // namespace Foundation
