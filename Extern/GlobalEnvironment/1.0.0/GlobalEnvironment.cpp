#include "GlobalEnvironment.h"

#include <inttypes.h>
#include <stdbool.h>
#include <shared_mutex>

#include <unordered_map>

namespace Foundation
{
class GlobalVariableAllocatorImpl : public GlobalVariableAllocatorInterface
{
 public:
   void* Allocate(uint64_t p_size)
   {
      return malloc(p_size);
   }

   void Deallocate(void* p_ptr)
   {
      free(p_ptr);
   }

 private:
};

class EnvironmentMapImplementation : public EnvironmentMapInterface
{
 public:
   EnvironmentMapImplementation() = default;

   GlobalVariableBase* GetGlobalVariable(uint64_t p_uuid) final
   {
      const auto& mapIt = m_environmentVariableMap.find(p_uuid);
      if (mapIt != m_environmentVariableMap.end())
      {
         return mapIt->second;
      }
      else
      {
         return nullptr;
      }
   }

   void RegisterGlobalVariable(GlobalVariableBase* p_globalVariable) final
   {
      // TODO: add an assert to chekc if p_uuid is valid
      m_environmentVariableMap[p_globalVariable->GetUuid()] = p_globalVariable;
   }

   void LockAndUnregisterGlobalVariable(uint64_t p_uuid) final
   {
      GlobalVariableAllocatorInterface* allocator = GetGlobalVariableAllocator();

      Lock();

      const auto& mapIt = m_environmentVariableMap.find(p_uuid);
      if (mapIt != m_environmentVariableMap.end())
      {
         GlobalVariableBase* globalVariable = mapIt->second;

         // Call the Destructor of the GlobalVariable, and free the memory manually
         globalVariable->~GlobalVariableBase();
         GetGlobalVariableAllocator()->Deallocate(static_cast<void*>(globalVariable));
      }
      else
      {
         int [[maybe_unused]] foo = 0;
      }

      Unlock();
   }

   GlobalVariableAllocatorInterface* GetGlobalVariableAllocator()
   {
      static GlobalVariableAllocatorImpl s_globalVariableAllocator;
      return &s_globalVariableAllocator;
   }

   void Lock()
   {
      m_mapMutex.lock();
   }

   void Unlock()
   {
      m_mapMutex.unlock();
   }

 private:
   std::unordered_map<uint64_t, GlobalVariableBase*> m_environmentVariableMap;
   std::recursive_mutex m_mapMutex;
};

EnvironmentMapInterface* GetGlobalEnvironmentMap()
{
   static EnvironmentMapImplementation s_environmentMap;
   return &s_environmentMap;
}
} // namespace Foundation
