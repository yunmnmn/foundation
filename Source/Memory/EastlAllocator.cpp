#include <Memory/EaStlAllocator.h>

#include <Util/Assert.h>
#include <Util/Util.h>

#include <Memory/MemoryManagerInterface.h>
#include <Memory/BaseSchema.h>

namespace Foundation
{
namespace Memory
{
EaStlAllocator::EaStlAllocator(const char* p_name)
    : BaseAllocator<EaStlAllocator, TlsfSchema>("EaStlAllocator", [this]() -> BaseSchema::Descriptor { return GetDescriptor(); })
{
}

EaStlAllocator::EaStlAllocator(const EaStlAllocator& p_other)
    : BaseAllocator<EaStlAllocator, TlsfSchema>("EaStlAllocator", [this]() -> BaseSchema::Descriptor { return GetDescriptor(); })
{
}

EaStlAllocator::EaStlAllocator(const EaStlAllocator& p_other, const char* p_name)
    : BaseAllocator<EaStlAllocator, TlsfSchema>("EaStlAllocator", [this]() -> BaseSchema::Descriptor { return GetDescriptor(); })
{
}

void* EaStlAllocator::allocate(size_t p_size, int32_t p_flag)
{
   return Allocate(p_size);
}

void* EaStlAllocator::allocate(size_t p_size, size_t p_alignment, size_t offset, int flags)
{
   return AllocateAllign(p_size, p_alignment);
}

void EaStlAllocator::deallocate(void* p, size_t n)
{
   Deallocate(p);
}

void* EaStlAllocator::AllocateInternal(uint32_t p_size)
{
   return ms_schema->Allocate(p_size);
}

void* EaStlAllocator::AllocateAlignInternal(uint32_t p_size, uint32_t p_alignment)
{
   // TODO FIx the parameters
   return ms_schema->AllocateAligned(p_size, p_alignment, 0);
}

void EaStlAllocator::DeallocateInternal(void* p_pointer)
{
   // TODO FIx the parameters
   ms_schema->Deallocate(p_pointer, 0);
}

BaseSchema::Descriptor EaStlAllocator::GetDescriptor()
{
   auto addPageLambda = [this](void* p_pageAddress) { AddPage(p_pageAddress); };
   auto removePageLambda = [this](void* p_pageAddress) { RemovePage(p_pageAddress); };

   BaseSchema::Descriptor desc = {1024u, 1024u * 1024u * 10u, addPageLambda, removePageLambda};
   return desc;
}

}; // namespace Memory
}; // namespace Foundation
