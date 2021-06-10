#pragma once

#include <EASTL/shared_ptr.h>
#include <Memory/BootstrapAllocator.h>

namespace Foundation
{
namespace Std
{

template <typename T>
struct DefaultDeleter
{
   DefaultDeleter() = default;

   template <typename U> // Enable if T* can be constructed with U* (i.e. U* is convertible to T*).
   DefaultDeleter(const struct DefaultDeleter<U>&, typename eastl::enable_if<eastl::is_convertible<U*, T*>::value>::type* = 0)
   {
   }

   void operator()(T* p) const
   {
      delete p;
   }
};

template <typename T>
eastl::shared_ptr<T> CreateSharedPtr(T* p_ptr)
{
   return eastl::shared_ptr<T>(p_ptr, DefaultDeleter<T>(), Foundation::Memory::EastlBootstrapAllocator());
}
}; // namespace Std
}; // namespace Foundation
