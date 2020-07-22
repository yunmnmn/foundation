#include <Memory/MemoryManager.h>

namespace Foundation
{
namespace Memory
{
void MemoryManager::RegisterAllocator(AllocatorBase* allocator)
{
   m_allocators.emplace_back(allocator);
}

AllocatorBase* Foundation::Memory::MemoryManager::GetAllocatorByName()
{
   return nullptr;
}

}; // namespace Memory
}; // namespace Foundation
