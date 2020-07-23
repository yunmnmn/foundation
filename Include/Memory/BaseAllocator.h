#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <functional>

#include <Memory/BootstrapAllocator.h>
#include <Memory/BaseSchema.h>
#include <Memory/TlsfSchema.h>

#include <Util/HashName.h>

#include <EASTL/array.h>
#include <EASTL/unordered_map.h>
#include <EASTL/unordered_set.h>
#include <EASTL/algorithm.h>

namespace Foundation
{
namespace Memory
{
   template <typename t_key, typename t_value>
   using unordered_map =
      eastl::unordered_map<t_key, t_value, eastl::hash<t_key>, eastl::equal_to<t_key>, BootstrapAllocator<TlsfSchema>, false>;

   template <typename t_value>
   using unordered_set =
      eastl::unordered_set<t_value, eastl::hash<t_value>, eastl::equal_to<t_value>, BootstrapAllocator<TlsfSchema>, false>;

struct Allocation
{
   uint8_t* m_allocationAddress = nullptr;
   uint64_t m_allocationSize = 0u;
};

struct Page
{
   uint8_t* m_pageAddress = nullptr;
   uint64_t m_pageSize = 0u;

   unordered_map<void*, Allocation> m_allocations;
};

class AllocatorInterface
{
 public:
   virtual uint32_t GetPageCount() const = 0;

 protected:
   virtual void* AllocateInternal(uint32_t p_size) = 0;
   virtual void* AllocateAlignInternal(uint32_t p_size, uint32_t p_alignment) = 0;
   virtual void DeallocateInternal(void* p_pointer) = 0;
   virtual void TrackAllocation(void* p_address, uint32_t p_size) = 0;
   virtual void UntrackAllocation(void* p_address) = 0;

   void* Allocate(uint32_t p_size);
   void* AllocateAllign(uint32_t p_size, uint32_t p_alignment);
   void Deallocate(void* p_address);
};

template <typename t_class, typename t_schema> class BaseAllocator : public AllocatorInterface
{
   // Schema allocation type
   using SchemaDataType = typename std::aligned_storage<sizeof(t_schema), std::alignment_of<t_schema>::value>::type;

 public:
   BaseAllocator(HashName p_allocatorName, std::function<BaseSchema::Descriptor()> p_getDescriptor)
   {
      // Initialize the schema
      CallOnce(ms_initializedFlag, [this, &p_allocatorName, &p_getDescriptor]() {
         // Register the allocator in the manager
         MemoryManagerInterface::Get()->RegisterAllocator(p_allocatorName, (AllocatorInterface*)this);

         // Create the schema descriptor
         BaseSchema::Descriptor desc = p_getDescriptor();

         // Create the Schema
         ms_schema = TlsfSchema::CreateSchema(desc, (void*)&ms_schemaData);

         // Set the name
         // ms_name = p_allocatorName;
      });

      ASSERT(ms_schema.get(), "Bootstrap schema isn't initialized");
   }

   uint32_t GetPageCount() const
   {
      return 0u;
   }

   void TrackAllocation(void* p_address, uint32_t p_size)
   {
      // Find the page where the allocation is placed
      auto pageIt = eastl::find_if(ms_pages.begin(), ms_pages.end(), [p_pageAddress](const Page& page) {
         if (p_address >= page.m_pageAddress)
         {
            if (p_address < page.m_pageAddress + page.m_pageSize)
            {
               return true;
            }
         }
         else
         {
            return false;
         }
         });
      ASSERT(pageIt != ms_pages.end(), "Allocation doesn't fit in the page");

      // Create the allocation, and push the allocation in the page
      Allocation alloc = {.m_allocationAddress = p_address, .m_allocationSize = p_size};
      pageIt->m_allocations[p_address] = alloc;


      auto allocation = ms_allocations.find(p_address);
      ASSERT(allocation == ms_allocations.end(), "Address is already used for an allocation");

      Allocation alloc = Allocation{.m_allocationAddress = (uint8_t*)p_address, .m_allocationSize = p_size};
      ms_allocations[p_address] = alloc;
   }

   void UntrackAllocation(void* p_address)
   {
      auto allocation = ms_allocations.find(p_address);
      ASSERT(allocation != ms_allocations.end(), "Address isn't used to allocate yet");

      ms_allocations.erase(allocation);
   }

   void AddPage(void* p_pageAddress, uint64_t p_pageSize)
   {
      // Make sure that the page isn't allocated already
      eastl::find_if(ms_pages.begin(), ms_pages.end(), [p_pageAddress](const Page& page) {
         ASSERT(page.m_pageAddress != p_pageAddress, "Page is already allocated");
         });

      ms_pages.emplace_back(p_pageAddress, p_pageSize);
   }

   void RemovePage(void* p_pageAddress)
   {
      auto pageIt = ms_pages.find(p_pageAddress);
      ASSERT(pageIt != ms_pages.end(), "Page doesn't exist");

      ms_pages.erase(pageIt);
   }

   // Allocation Address -> Allocation
   static vector<Page> ms_pages;

   static eastl::unique_ptr<t_schema> ms_schema;
   static bool ms_initializedFlag;
   static SchemaDataType ms_schemaData;
   static HashName ms_name;
};

//template <typename t_class, typename t_schema>
//BaseAllocator<t_class, t_schema>::unordered_map<void*, Allocation> BaseAllocator<t_class, t_schema>::ms_allocations;
template <typename t_class, typename t_schema>
BaseAllocator<t_class, t_schema>::vector<Page> BaseAllocator<t_class, t_schema>::ms_pages;

template <typename t_class, typename t_schema> eastl::unique_ptr<t_schema> BaseAllocator<t_class, t_schema>::ms_schema;
template <typename t_class, typename t_schema> bool BaseAllocator<t_class, t_schema>::ms_initializedFlag = false;
template <typename t_class, typename t_schema>
typename BaseAllocator<t_class, t_schema>::SchemaDataType BaseAllocator<t_class, t_schema>::ms_schemaData;
template <typename t_class, typename t_schema> HashName BaseAllocator<t_class, t_schema>::ms_name;

}; // namespace Memory
}; // namespace Foundation
