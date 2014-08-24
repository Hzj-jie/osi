
#pragma once
#include <type_traits>
#include <stdlib.h>

namespace __recursive_deleter_private
{
    // prototype
    template <typename T>
    static void recursive_delete(T* i);

    template <typename T>
    static void recursive_delete(T& i);

    template <typename T, size_t N>
    static void recursive_delete(T* i);

    // implementation
    template <typename T>
    static void recursive_delete(T* i)
    {
        // T = U*
        // typeof i == U**
        // typeof (*i) == U*
        recursive_delete<typename std::remove_pointer<T>::type>(*i);
        delete[] i;
    }

    template <typename T>
    static void recursive_delete(T& i)
    {
        if(std::is_pointer<T>::value)
        {
            // T = U*
            // typeof i = U*
            recursive_delete<typename std::remove_pointer<T>::type>(i);
        }
        else { } // do nothing
    }

    template <typename T, size_t N>
    static void recursive_delete(T* i)
    {
        for(size_t j = 0; j < N; j++)
            recursive_delete(i[j]);
        delete[] i;
    }
}

template <typename T, size_t N = 0>
struct recursive_deleter
{
public:
    void operator()(T& i) const
    {
        __recursive_deleter_private::recursive_delete(i);
    }

    void operator()(T* i) const
    {
        __recursive_deleter_private::recursive_delete<T, N>(i);
    }
};

