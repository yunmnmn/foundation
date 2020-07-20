#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <Memory/Memory.h>
#include <Memory/TlsfAllocator.h>

namespace Foundation
{
namespace Memory
{
struct StdDescriptor
{
};

template <typename t_type, typename t_allocator>
class StdAllocatorInterface
{
   using value_type = T;
   using pointer = T*;
   using const_pointer = const T*;
   using reference = T&;
   using const_reference = const T&;
   using size_type = std::size_t;
   using difference_type = std::ptrdiff_t;

   template <class U>
   struct rebind
   {
      using other = StdAllocator<U>;
   };

   size_type max_size() const()
   {
      return std::numeric_limits<std::size_t>::max() / sizeof(T);
   }

   T* allocate(std::size_t n) noexcept
   {
      void* data = t_allocator::Allocate();
      return data;
   }

   void deallocate(T* p, std::size_t) noexcept
   {
      std::free(p);
   }

   // initialize elements of allocated storage p with value value
   void construct(pointer p, const T& value)
   {
      // initialize memory with placement new
      new ((void*)p) T(value);
   }

   // destroy elements of initialized storage p
   void destroy(pointer p)
   {
      // destroy objects by calling their destructor
      p->~T();
   }
};

}; // namespace Memory
}; // namespace Foundation

namespace YStd
{
using StdAllocatorTlsf =
    Foundation::Memory::AllocatorInterface<Foundation::Memory::StdDescriptor, Foundation::Memory::TlsfAllocator>;

template <typename t_type, typename t_allocator = Foundation::Memory::StdAllocatorInterface<t_type, StdAllocatorTlsf>>
using vector = EAStl::vector<t_type, t_allocator>;

} // namespace YStd
