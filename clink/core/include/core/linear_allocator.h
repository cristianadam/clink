// Copyright (c) 2016 Martin Ridgers
// License: http://opensource.org/licenses/MIT

#pragma once

//------------------------------------------------------------------------------
class linear_allocator
{
public:
                            linear_allocator(unsigned int size);
                            linear_allocator(linear_allocator&& o) = delete;
                            ~linear_allocator();
    linear_allocator&       operator = (linear_allocator&& o);
    void                    reset();
    void                    clear();
    void*                   alloc(unsigned int size);
    const char*             store(const char* str);
    template <class T> T*   calloc(unsigned int count=1);
    bool                    fits(unsigned int) const;
    bool                    oversized(unsigned int) const;

    bool                    unittest_at_end(void* ptr, unsigned int size) const;
    bool                    unittest_in_prev_page(void* ptr, unsigned int size) const;

private:
    bool                    new_page();
    void                    free_chain(bool keep_one=false);
    char*                   m_ptr = nullptr;
    unsigned int            m_used;
    unsigned int            m_max;
};

//------------------------------------------------------------------------------
inline void linear_allocator::reset()
{
    free_chain(true/*keep_one*/);
}

//------------------------------------------------------------------------------
inline void linear_allocator::clear()
{
    free_chain(false/*keep_one*/);
}

//------------------------------------------------------------------------------
template <class T> T* linear_allocator::calloc(unsigned int count)
{
    void* p = alloc(sizeof(T) * count);
    if (p)
        memset(p, 0, sizeof(T) * count);
    return static_cast<T*>(p);
}

//------------------------------------------------------------------------------
inline bool linear_allocator::fits(unsigned int size) const
{
    return m_used + size <= m_max;
}

//------------------------------------------------------------------------------
inline bool linear_allocator::oversized(unsigned int size) const
{
    return size + sizeof(m_ptr) > m_max;
}



//------------------------------------------------------------------------------
inline bool linear_allocator::unittest_at_end(void* ptr, unsigned int size) const
{
    return ptr == m_ptr + m_used - size;
}

//------------------------------------------------------------------------------
inline bool linear_allocator::unittest_in_prev_page(void* _ptr, unsigned int size) const
{
    char* ptr = (char*)_ptr;
    char* prev_page = *reinterpret_cast<char**>(m_ptr);
    if (oversized(size))
        return ptr == prev_page + sizeof(m_ptr);
    return ptr >= prev_page + sizeof(m_ptr) && ptr + size <= prev_page + m_max;
}
