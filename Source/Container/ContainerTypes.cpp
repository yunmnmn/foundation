#include <Container/ContainerTypes.h>

namespace Foundation
{
namespace Container
{
//-----------------------------------------------------------------------------
const uint64_t KiloByte = sizeof(uint8_t) * 1024u;
const uint64_t MegaByte = KiloByte * 1024u;
const uint64_t ResourceMemorySize = MegaByte * 12u;
//-----------------------------------------------------------------------------
Foundation::Memory::TlsfAllocator
    Allocators::ms_DynamicResourceAllocator(ResourceMemorySize);
//-----------------------------------------------------------------------------
}; // namespace Container
}; // namespace Foundation
