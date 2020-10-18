#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <type_traits>

#include <EASTL/unique_ptr.h>

#include <Util/Assert.h>
#include <Util/Util.h>

#include <Memory/BaseSchema.h>
#include <Memory/TlsfSchema.h>

namespace Foundation
{
namespace Memory
{
// General BootstrapAllocator which is available before any code is executed. Memory isn't tracked
// Uses the TlsfSchema
class BootstrapAllocator
{
   using BootstrapSchema = TlsfSchema<1u, 4u * 1024 * 1000u>;

   BootstrapAllocator() = delete;

 public:
   static void* Allocate(uint32_t p_size)
   {
      AllocationDescriptor desc = ms_schema.Allocate(p_size);
      return desc.m_address;
   }

   static void* AllocateAllign(uint32_t p_size, uint32_t p_alignment)
   {
      AllocationDescriptor desc = ms_schema.AllocateAligned(p_size, p_alignment, 0u);
      return desc.m_address;
   }

   static void Deallocate(void* p_address, uint32_t p_size)
   {
      ms_schema.Deallocate(p_address, p_size);
   }

 private:
   static BootstrapSchema ms_schema;
};

// BootstrapAllocator for EASTL containers
class EastlBootstrapAllocator
{
 public:
   EastlBootstrapAllocator(const char* p_name)
   {
   }

   EastlBootstrapAllocator(const EastlBootstrapAllocator& p_other)
   {
   }

   EastlBootstrapAllocator(const EastlBootstrapAllocator& p_other, const char* p_name)
   {
   }

   bool operator!=(const EastlBootstrapAllocator& other)
   {
      // Only one bootstrap allocator, so always return true
      return true;
   }

   static void* allocate(size_t p_size, int32_t p_flag = 0)
   {
      return BootstrapAllocator::Allocate(static_cast<uint32_t>(p_size));
   }

   static void* allocate(size_t p_size, size_t p_alignment, size_t p_offset, int p_flags = 0)
   {
      return BootstrapAllocator::AllocateAllign(static_cast<uint32_t>(p_size), static_cast<uint32_t>(p_alignment));
   }

   static void deallocate(void* p_address, size_t p_size)
   {
      BootstrapAllocator::Deallocate(p_address, static_cast<uint32_t>(p_size));
   }
};

}; // namespace Memory
}; // namespace Foundation
