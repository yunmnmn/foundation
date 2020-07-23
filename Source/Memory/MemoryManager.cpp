#include <Memory/MemoryManager.h>

namespace Foundation
{
namespace Memory
{
void MemoryManager::RegisterAllocator(HashName p_hashName, AllocatorInterface* p_allocator)
{
   m_allocators[p_hashName.hash()] = p_allocator;
}

void MemoryManager::UnregisterAllocator(HashName p_hashName)
{
}

}; // namespace Memory
}; // namespace Foundation
