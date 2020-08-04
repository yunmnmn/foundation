#include <Parallel/SpinLock.h>

namespace Foundation
{

// SpinLock

void SpinLock::Lock()
{
   while (!m_flag.test_and_set())
   {
   }
}

void SpinLock::Unlock()
{
   m_flag.clear();
}

}; // namespace Foundation
