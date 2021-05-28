#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <typeinfo>

// TODO: move this
#ifdef DLL_EXPORTS
/*Enabled as "export" while compiling the dll project*/
#define FOUNDATION_API extern "C" __declspec(dllexport)
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define FOUNDATION_API extern "C" __declspec(dllimport)
#endif

namespace Foundation
{
class EnvironmentMapInterface;

FOUNDATION_API EnvironmentMapInterface* GetGlobalEnvironmentMap();

class EnvironmentMapInterface
{
 public:
   virtual void* GetGlobalVariable(uint64_t p_uuid) = 0;
   virtual void RegisterGlobalVariable(uint64_t p_uuid, void* p_globalVariable) = 0;
   virtual void* UnregisterGlobalVariable(uint64_t p_uuid) = 0;

   virtual void Lock() = 0;
   virtual void Unlock() = 0;

 private:
};

class GlobalEnvironment
{
 public:
   // Creates or retrieves the Global variable
   template <typename t_variable, typename t_functor>
   static t_variable* CreateOrGetGlobalVariableFromType(t_functor&& p_creationFunctor)
   {
      EnvironmentMapInterface* globalEnvironmentMap = GetGlobalEnvironmentMap();

      globalEnvironmentMap->Lock();

      const uint64_t uuid = typeid(t_variable).hash_code();
      void* globalVariable = globalEnvironmentMap->GetGlobalVariable(uuid);

      // Global variable doesn't exist yet
      if (!globalVariable)
      {
         void* newGlobalVariable = reinterpret_cast<void*>(p_creationFunctor());
         globalEnvironmentMap->RegisterGlobalVariable(uuid, newGlobalVariable);
         return reinterpret_cast<t_variable*>(newGlobalVariable);
      }
      else
      {
         return reinterpret_cast<t_variable*>(globalVariable);
      }

      globalEnvironmentMap->Unlock();
   }

   // Creates or retrieves the Global variable
   template <typename t_variable, typename t_functor>
   static t_variable* CreateOrGetGlobalVariableFromUuid(uint64_t p_uuid, t_functor&& p_creationFunctor)
   {
      EnvironmentMapInterface* globalEnvironmentMap = GetGlobalEnvironmentMap;
      void* globalVariable = globalEnvironmentMap->GetGlobalVariable(p_uuid);

      // Global variable doesn't exist yet
      if (!globalVariable)
      {
         void* newGlobalVariable = p_creationFunctor();
         globalEnvironmentMap->RegisterGlobalVariable(newGlobalVariable);
         return reinterpret_cast<t_variable*>(newGlobalVariable);
      }
      else
      {
         return reinterpret_cast<t_variable*>(globalVariable);
      }
   }

 private:
};

// Extern C, and export two functions to
} // namespace Foundation
