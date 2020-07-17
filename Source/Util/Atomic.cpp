// #include "Atomic.h"

namespace Foundation
{
namespace
{
namespace Internal
{
////---------------------------------------------------------------------------------//
// const uint32_t MaxAtomicCounters = 128u;
////---------------------------------------------------------------------------------//
// template <size_t ResourceSize, typename ResourceHandle>
// struct BaseData
//{
//  static void getByHandle(ResourceHandle p_Handle)
//  {
//  }
//  //---------------------------------------------------------------------------------//
//  static ResourceHandle generateData()
//  {
//    return resourceCounter++;
//  }
//  //---------------------------------------------------------------------------------//
//  static uint32_t resourceCounter;
//  static uint32_t freeResourceList[ResourceSize];
//};
////---------------------------------------------------------------------------------//
// template <typename ResourceType, size_t ResourceSize>
// struct BaseResource
//{
//  static ResourceType data[ResourceSize];
//};
////---------------------------------------------------------------------------------//
// struct AtomicCounterData
//    : BaseData<MaxAtomicCounters, Atomic::AtomicCounterHandle>
//{
//  BaseResource<uint32_t, MaxAtomicCounters> testData;
//  // atomic<uint32_t> atomicUInt[MaxAtomicCounters];
//} g_Data;
////---------------------------------------------------------------------------------//
//} // namespace Internal
//} // namespace
//
// namespace Atomic
//{
////---------------------------------------------------------------------------------//
// void AtomicCounter::createAtomicCounter(
//    AtomicCounterHandle* const p_AtomicCounterHandle, uint32_t p_Count)
//{
//  for (uint32_t i = 0u; i < p_Count; i++) {
//    p_AtomicCounterHandle[i] = Internal::g_Data::generateData();
//  }
//}
////---------------------------------------------------------------------------------//
//
// void AtomicCounter::createAtomicCounter(AtomicCounterHandle* p_AtomicCounter,
//                                        uint32_t p_Count)
//{
//}
////---------------------------------------------------------------------------------//
// void GetAtomicValue()
//{
//}
////---------------------------------------------------------------------------------//
// void CompareAtomicValue();
//---------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------//
} // namespace Internal
}; // namespace
}; // namespace Foundation
