#include <Memory/MemoryManager.h>

#include <Util/Assert.h>

namespace Foundation
{
namespace Memory
{
void MemoryManager::RegisterAllocator(HashName p_hashName, BaseAllocator* p_allocator)
{
   std::lock_guard<std::mutex> guard(m_registrationMutex);

   ASSERT(p_allocator != nullptr, "Allocator must be valid");

   // Check if the name is already in the map
   auto& allocator = m_allocators.find(p_hashName.Hash());
   ASSERT(allocator == m_allocators.end(), "Allocator already exists");

   m_allocators[p_hashName.Hash()] = p_allocator;
}

void MemoryManager::UnregisterAllocator(HashName p_hashName)
{
   std::lock_guard<std::mutex> guard(m_registrationMutex);

   // Check if the name is already in the map
   auto& allocator = m_allocators.find(p_hashName.Hash());
   ASSERT(allocator != m_allocators.end(), "Allocator is not in the map");

   m_allocators.erase(allocator);
}

}; // namespace Memory
}; // namespace Foundation
