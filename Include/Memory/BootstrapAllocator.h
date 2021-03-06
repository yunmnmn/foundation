#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Util/Assert.h>
#include <Util/Macro.h>

#include <GlobalEnvironment.h>

#include <Memory/TlsfSchema.h>

namespace Foundation
{
namespace Memory
{
// General BootstrapAllocator which is available before any code is executed. Memory isn't tracked
// Uses the TlsfSchema
class BootstrapAllocator
{
   // NOTE: Max size allocated from a page is 4 MB
   using BootstrapSchema = TlsfSchema<12u, 4u * 1024000u>;

   BootstrapAllocator() = delete;

 public:
   static void* Allocate(uint32_t p_size)
   {
      AllocationDescriptor desc = GetSchema().Allocate(p_size);
      return desc.m_address;
   }

   static void* AllocateAllign(uint32_t p_size, uint32_t p_alignment)
   {
      AllocationDescriptor desc = GetSchema().AllocateAligned(p_size, p_alignment, 0u);
      return desc.m_address;
   }

   static void Deallocate(void* p_address, uint32_t p_size)
   {
      GetSchema().Deallocate(p_address, p_size);
   }

   static BootstrapSchema& GetSchema()
   {
      static GlobalVariableRef<BootstrapSchema> schema =
          Foundation::GlobalEnvironment::CreateOrGetGlobalVariableFromType<BootstrapSchema>();
      return *schema.GetVariable();
   }

 private:
};

// BootstrapAllocator for EASTL containers
class EastlBootstrapAllocator
{
 public:
   EastlBootstrapAllocator() = default;
   EastlBootstrapAllocator([[maybe_unused]] const char* p_name)
   {
   }

   EastlBootstrapAllocator([[maybe_unused]] const EastlBootstrapAllocator& p_other)
   {
   }

   EastlBootstrapAllocator([[maybe_unused]] const EastlBootstrapAllocator& p_other, [[maybe_unused]] const char* p_name)
   {
   }

   bool operator!=([[maybe_unused]] const EastlBootstrapAllocator& other)
   {
      // Only one bootstrap allocator, so always return true, even across DLLs
      return true;
   }

   static void* allocate(size_t p_size, [[maybe_unused]] int32_t p_flag = 0)
   {
      return BootstrapAllocator::Allocate(static_cast<uint32_t>(p_size));
   }

   static void* allocate(size_t p_size, size_t p_alignment, size_t p_offset, int p_flags = 0)
   {
      UNUSED(p_offset);
      UNUSED(p_flags);
      return BootstrapAllocator::AllocateAllign(static_cast<uint32_t>(p_size), static_cast<uint32_t>(p_alignment));
   }

   static void deallocate(void* p_address, size_t p_size)
   {
      BootstrapAllocator::Deallocate(p_address, static_cast<uint32_t>(p_size));
   }
};

}; // namespace Memory
}; // namespace Foundation
