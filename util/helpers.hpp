#ifndef UTIL__HELPERS_H_
#define UTIL__HELPERS_H_

// workaround for final not being present in gcc < 4.7
#if defined(__GNUG__) && __GNUC_MINOR__ < 7
#define final 
#define override 
#elif defined(_MSC_VER) && _MSC_VER <= 1600
#define final sealed
#endif

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
