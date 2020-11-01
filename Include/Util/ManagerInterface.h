#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <shared_mutex>

#include <Util/Assert.h>

namespace Foundation
{
namespace Util
{
template <typename t_managerInterface>
class ManagerInterface
{
 public:
   static t_managerInterface* Get()
   {
      std::shared_lock<std::shared_mutex> lock(ms_registerMutex);

      return ms_managerInterface;
   }

   static void Register(t_managerInterface* p_managerInterface)
   {
      std::unique_lock<std::shared_mutex> lock(ms_registerMutex);

      ASSERT(p_managerInterface, "Trying to register an unvalid pointer");
      ms_managerInterface = p_managerInterface;
   }

   static void Unregister()
   {
      std::unique_lock<std::shared_mutex> lock(ms_registerMutex);

      ASSERT(ms_managerInterface, "Trying to unregister an unvalid pointer");
      ms_managerInterface = nullptr;
   }

 private:
   static inline t_managerInterface* ms_managerInterface = nullptr;
   static inline std::shared_mutex ms_registerMutex;
};

}; // namespace Util
}; // namespace Foundation
