#pragma once

namespace Foundation
{
//-----------------------------------------------------------------------------
template <typename Functor> void callOnce(Functor functor)
{
  static bool called = false;
  if (!called)
  {
    functor();
    called = true;
  }
}
//-----------------------------------------------------------------------------
} // namespace Foundation
