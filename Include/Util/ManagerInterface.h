#pragma once

#include <inttypes.h>
#include <stdbool.h>

namespace Foundation
{
namespace Util
{
template <typename t_managerInterface> class ManagerInterface
{
 public:
   static t_managerInterface* Get()
   {
      ASSERT(ms_managerInterface, "No manager has been registered yet");
      return ms_managerInterface;
   }

   static void Register(t_managerInterface* p_managerInterface)
   {
      ASSERT(!p_managerInterface, "Trying to register an unvalid pointer");
      ms_managerInterface = p_managerInterface;
   }

 private:
   static inline t_managerInterface* ms_managerInterface = nullptr;
};

}; // namespace Util
}; // namespace Foundation
