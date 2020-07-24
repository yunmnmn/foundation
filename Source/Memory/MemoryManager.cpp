#include <Memory/MemoryManager.h>

#include <Util/Assert.h>

namespace Foundation
{
namespace Memory
{
void MemoryManager::RegisterAllocator(HashName p_hashName, BaseAllocator* p_allocator)
{
   ASSERT(p_allocator != nullptr, "Allocator must be valid");

   // Check if the name is already in the map
   auto allocator = m_allocators.find(p_hashName.hash());
   ASSERT(allocator == m_allocators.end(), "Allocator already exists");

   m_allocators[p_hashName.hash()] = p_allocator;
}

void MemoryManager::UnregisterAllocator(HashName p_hashName)
{
   // Check if the name is already in the map
   auto allocator = m_allocators.find(p_hashName.hash());
   ASSERT(allocator != m_allocators.end(), "Allocator is not in the map");

   m_allocators.erase(allocator);
}

}; // namespace Memory
}; // namespace Foundation
