#include "GlobalEnvironment.h"

#include <inttypes.h>
#include <stdbool.h>
#include <shared_mutex>

#include <unordered_map>

namespace Foundation
{
struct GlobalVariable
{
   void* m_variable;
};

class EnvironmentMapImplementation : public EnvironmentMapInterface
{
 public:
   EnvironmentMapImplementation() = default;

   void* GetGlobalVariable(uint64_t p_uuid) final
   {
      // TODO: add mutex
      const auto& mapIt = m_environmentVariableMap.find(p_uuid);
      if (mapIt != m_environmentVariableMap.end())
      {
         return mapIt->second.m_variable;
      }
      else
      {
         return nullptr;
      }
   }

   void RegisterGlobalVariable(uint64_t p_uuid, void* p_globalVariable) final
   {
      m_environmentVariableMap[p_uuid] = GlobalVariable{.m_variable = p_globalVariable};
   }

   void* UnregisterGlobalVariable(uint64_t p_uuid) final
   {
      return nullptr;
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
   std::unordered_map<uint64_t, GlobalVariable> m_environmentVariableMap;
   std::recursive_mutex m_mapMutex;
};

static EnvironmentMapImplementation s_environmentMap;

EnvironmentMapInterface* GetGlobalEnvironmentMap()
{
   return &s_environmentMap;
}
} // namespace Foundation
