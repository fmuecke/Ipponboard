#ifndef UTIL__HELPERS_H_
#define UTIL__HELPERS_H_

namespace fmu
{

template<typename T, int size>
inline int ArrayLength(T(&)[size])
{
    return size;
}

template<typename T>
inline bool IsOptionSet(T data, T option)
{
    return (data & option) == option;
}

} // namespace

#endif  // UTIL__HELPERS_H_
