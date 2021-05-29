#pragma once

#include <inttypes.h>
#include <stdbool.h>

#include <typeinfo>
#include <atomic>

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
class GlobalVariableBase;

// Interface to the EnvironmentMap
class EnvironmentMapInterface
{
 public:
   virtual GlobalVariableBase* GetGlobalVariable(uint64_t p_uuid) = 0;
   virtual void RegisterGlobalVariable(GlobalVariableBase* p_globalVariable) = 0;
   virtual void LockAndUnregisterGlobalVariable(uint64_t p_uuid) = 0;

   virtual class GlobalVariableAllocatorInterface* GetGlobalVariableAllocator() = 0;

   virtual void Lock() = 0;
   virtual void Unlock() = 0;
};

FOUNDATION_API EnvironmentMapInterface* GetGlobalEnvironmentMap();

// Allocator to use for initializing the GlobalEnvironment variables. These will call the native platform's malloc and free
class GlobalVariableAllocatorInterface
{
 public:
   virtual void* Allocate(uint64_t p_size) = 0;
   virtual void Deallocate(void* p_ptr) = 0;

 private:
};

class GlobalVariableBase
{
 public:
   GlobalVariableBase() = delete;

   GlobalVariableBase(uint64_t p_uuid)
   {
      m_uuid = p_uuid;
   }

   virtual ~GlobalVariableBase() = default;

   void AddRef()
   {
      m_refCount++;
   }

   void ReleaseRef()
   {
      if (--m_refCount <= 0)
      {
         RefCountZero();
      }
   }

   const uint64_t GetUuid() const
   {
      return m_uuid;
   }

   virtual void RefCountZero() = 0;

 private:
   std::atomic_uint32_t m_refCount = 0u;
   uint64_t m_uuid = 0u;
};

template <typename t_variableType>
class GlobalVariable : GlobalVariableBase
{
   template <typename t_variableType>
   friend class GlobalVariableRef;

   friend class GlobalEnvironment;

 public:
   virtual ~GlobalVariable()
   {
      // Call the destructor of the variable, and free the memory manually
      m_variable->~t_variableType();
      m_variableAllocator->Deallocate(m_variable);
   }

 private:
   GlobalVariable(uint64_t p_uuid, GlobalVariableAllocatorInterface* p_variableAllocator) : GlobalVariableBase(p_uuid)
   {
      m_variableAllocator = p_variableAllocator;

      // Allocate the variable
      void* variableMemory = m_variableAllocator->Allocate(sizeof(t_variableType));
      m_variable = new (variableMemory) t_variableType();
   }

   void RefCountZero()
   {
      // Remove the GlobalVariable instance from the GlobalEnvironment
      EnvironmentMapInterface* globalEnvironmentMap = GetGlobalEnvironmentMap();
      globalEnvironmentMap->LockAndUnregisterGlobalVariable(GetUuid());
   }

   t_variableType* m_variable = nullptr;
   GlobalVariableAllocatorInterface* m_variableAllocator = nullptr;
};

template <typename t_variableType>
class GlobalVariableRef
{
   friend class GlobalEnvironment;

   using GlobalVariableType = GlobalVariable<t_variableType>;

 public:
   ~GlobalVariableRef()
   {
      m_globalVariable->ReleaseRef();
   }

   GlobalVariableType* operator->()
   {
      return m_globalVariable.get();
   }

   const GlobalVariableType* operator->() const
   {
      return m_globalVariable.get();
   }

   GlobalVariableType* Get()
   {
      return m_globalVariable.get();
   }

   const GlobalVariableType* Get() const
   {
      return m_globalVariable.get();
   }

   t_variableType* GetVariable()
   {
      return m_globalVariable->m_variable;
   }

 private:
   GlobalVariableRef() = delete;
   GlobalVariableRef(GlobalVariableType* p_globalVariable)
   {
      m_globalVariable = p_globalVariable;

      p_globalVariable->AddRef();
   }

   GlobalVariableType* m_globalVariable = nullptr;
};

class GlobalEnvironment
{
 public:
   // Creates or retrieves the Global variable
   template <typename t_variableType>
   static GlobalVariableRef<t_variableType> CreateOrGetGlobalVariableFromType()
   {
      const uint64_t uuid = typeid(t_variableType).hash_code();

      return CreateOrGetGlobalVariableFromUuid<t_variableType>(uuid);
   }

   // Creates or retrieves the Global variable
   template <typename t_variableType>
   static GlobalVariableRef<t_variableType> CreateOrGetGlobalVariableFromUuid(uint64_t p_uuid)
   {
      using GlobalVariableType = GlobalVariable<t_variableType>;

      EnvironmentMapInterface* globalEnvironmentMap = GetGlobalEnvironmentMap();

      globalEnvironmentMap->Lock();

      GlobalVariableBase* globalVariable = globalEnvironmentMap->GetGlobalVariable(p_uuid);

      // Global variable doesn't exist yet
      if (!globalVariable)
      {
         GlobalVariableAllocatorInterface* allocator = globalEnvironmentMap->GetGlobalVariableAllocator();

         void* variableMemory = allocator->Allocate(sizeof(GlobalVariable<t_variableType>));
         GlobalVariable<t_variableType>* newGlobalVariable = new (variableMemory) GlobalVariable<t_variableType>(p_uuid, allocator);

         globalEnvironmentMap->RegisterGlobalVariable(newGlobalVariable);

         globalEnvironmentMap->Unlock();

         return newGlobalVariable;
      }
      else
      {
         globalEnvironmentMap->Unlock();

         return reinterpret_cast<GlobalVariableType*>(globalVariable);
      }
   }

 private:
};

// Extern C, and export two functions to
} // namespace Foundation
