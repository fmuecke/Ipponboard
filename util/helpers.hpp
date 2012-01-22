#ifndef UTIL__HELPERS_H_
#define UTIL__HELPERS_H_

namespace fmu
{

template<typename T, int size>
int ArrayLength(T(&)[size]){return size;}

template<typename T1, typename T2>
bool IsOptionSet(T1 data, T2 option)
{
    return (data & option) == option;
}

} // namespace

#endif  // UTIL__HELPERS_H_
