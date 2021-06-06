#include <Memory/BootstrapAllocator.h>

void* operator new[]([[maybe_unused]] size_t size, [[maybe_unused]] const char* pName, [[maybe_unused]] int flags,
                     [[maybe_unused]] unsigned debugFlags, [[maybe_unused]] const char* file, [[maybe_unused]] int line)
{
   return Foundation::Memory::BootstrapAllocator::Allocate(static_cast<uint32_t>(size));
}
void* operator new[]([[maybe_unused]] size_t size, [[maybe_unused]] size_t alignment, [[maybe_unused]] size_t alignmentOffset,
                     [[maybe_unused]] const char* pName, [[maybe_unused]] int flags, [[maybe_unused]] unsigned debugFlags,
                     [[maybe_unused]] const char* file [[maybe_unused]], [[maybe_unused]] int line)
{
   return Foundation::Memory::BootstrapAllocator::AllocateAllign(static_cast<uint32_t>(size), static_cast<uint32_t>(alignment));
}
