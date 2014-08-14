
#pragma once
#include <boost/lockfree/queue.hpp>

template <typename T>
class slimqless2 : public boost::lockfree::queue<T>
{
public:
    slimqless2() : boost::lockfree::queue<T>(1024) { }
    /*
    template<typename U> 
    explicit slimqless2(typename boost::lockfree::queue<T>::node_allocator::template rebind< U >::other const & v) :
        boost::lockfree::queue<T>(v) { }
    explicit slimqless2(boost::lockfree::queue<T>::allocator const & a) :
        boost::lockfree::queue<T>(a) { }
    explicit slimqless2(boost::lockfree::queue<T>::size_type s) :
        boost::lockfree::queue<T>(s) { }
    template<typename U> 
    slimqless2(boost::lockfree::queue<T>::size_type s, 
               typename boost::lockfree::queue<T>::node_allocator::template rebind< U >::other const & v) :
        boost::lockfree::queue<T>(s, v) { }
    */

    slimqless2(const slimqless2&) = default;
    slimqless2& operator=(const slimqless2&) = default;

    void clear()
    {
        T v;
        while(boost::lockfree::queue<T>::pop(v));
    }
};

