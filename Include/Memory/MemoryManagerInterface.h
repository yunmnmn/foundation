#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Util/Assert.h>
#include <Util/ManagerInterface.h>

namespace Foundation
{
namespace Memory
{
class BaseAllocator;

class MemoryManagerInterface : public Util::ManagerInterface<MemoryManagerInterface>
{
 public:
   virtual void RegisterAllocator(BaseAllocator* allocator) = 0;
   virtual void UnregisterAllocator() = 0;
};

}; // namespace Memory
}; // namespace Foundation
