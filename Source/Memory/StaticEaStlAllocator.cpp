#include <Memory/StaticEaStlAllocator.h>

#include <Util/Assert.h>

namespace Foundation
{
namespace Memory
{
// Declare the static members
eastl::unique_ptr<BaseAllocator> StaticEaStlAllocator::ms_allocator;
bool StaticEaStlAllocator::ms_initializedFlag = false;

StaticEaStlAllocator::StaticEaStlAllocator(const char* p_name)
{
   initializeAllocator();
}

StaticEaStlAllocator::StaticEaStlAllocator(const StaticEaStlAllocator& p_other)
{
}

StaticEaStlAllocator::StaticEaStlAllocator(const StaticEaStlAllocator& p_other, const char* p_name)
{
}

void* StaticEaStlAllocator::allocate(size_t p_size, int32_t p_flag)
{
   ASSERT(ms_allocator != nullptr, "Allocator isn't initialized");
   return ms_allocator->Allocate(p_size);
}

void* StaticEaStlAllocator::allocate(size_t p_size, size_t p_alignment, size_t p_offset, int p_flags)
{
   ASSERT(ms_allocator != nullptr, "Allocator isn't initialized");
   return ms_allocator->AllocateAllign(p_size, p_alignment);
}

void StaticEaStlAllocator::deallocate(void* p_address, size_t p_size)
{
   ASSERT(ms_allocator != nullptr, "Allocator isn't initialized");
   ms_allocator->Deallocate(p_address, p_size);
}

void StaticEaStlAllocator::initializeAllocator()
{
   CallOnce(ms_initializedFlag, []() {
      const uint32_t PageCount = 1024u;
      const uint64_t PageSize = 1024 * 1024 * 10u;
      ms_allocator = eastl::move(EaStlAllocator<PageCount, PageSize>::CreateAllocator("StaticEaStlAllocator"));
   });
}
}; // namespace Memory
}; // namespace Foundation
