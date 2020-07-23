#include <Memory/MemoryManager.h>

namespace Foundation
{
namespace Memory
{
void MemoryManager::RegisterAllocator(BaseAllocator* allocator)
{
   m_allocators.emplace_back(allocator);
}

void MemoryManager::UnregisterAllocator()
{
}

BaseAllocator* Foundation::Memory::MemoryManager::GetAllocatorByName()
{
   return nullptr;
}

}; // namespace Memory
}; // namespace Foundation
