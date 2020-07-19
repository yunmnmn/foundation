#pragma once

class EaStdAllocator
{
 public:
   EASTL_ALLOCATOR_EXPLICIT allocator(const char* pName = EASTL_NAME_VAL(EASTL_ALLOCATOR_DEFAULT_NAME));
   allocator(const allocator& x);
   allocator(const allocator& x, const char* pName);

   allocator& operator=(const allocator& x);

   void* allocate(size_t n, int flags = 0);
   void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0);
   void deallocate(void* p, size_t n);

   const char* get_name() const;
   void set_name(const char* pName);

 protected:
};
