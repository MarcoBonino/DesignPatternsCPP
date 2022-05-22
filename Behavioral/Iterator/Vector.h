#pragma once

#include <cstddef>
#include <new>
#include <utility>

template <typename VECTOR>
class VectorIterator
{
public:
    using ValueType = typename VECTOR::ValueType;
    using PointerType = ValueType*;
    using ReferenceType = ValueType&;

public:
    explicit VectorIterator(PointerType ptr)
    : m_ptr(ptr)
    {}

    VectorIterator& operator++() // pre-increment
    {
        ++m_ptr;
        return *this;
    }

    VectorIterator operator++(int) // post-increment
    {
        VectorIterator copy = *this;
        ++m_ptr;
        return copy;
    }

    VectorIterator& operator--() // pre-decrement
    {
        --m_ptr;
        return *this;
    }

    VectorIterator operator--(int) // post-decrement
    {
        VectorIterator copy = *this;
        --m_ptr;
        return copy;
    }

    ReferenceType operator[](size_t index)
    {
        return *(m_ptr+index);
    }

    PointerType operator->()
    {
        return m_ptr;
    }

    ReferenceType operator*()
    {
        return *m_ptr;
    }

    bool operator==(const VectorIterator& other) const
    {
        return (m_ptr == other.m_ptr);
    }

    bool operator!=(const VectorIterator& other) const
    {
        return (m_ptr != other.m_ptr);
    }

private:
    PointerType m_ptr;
};

template <typename T>
class Vector
{
public:
    using ValueType = T;
    using Iterator = VectorIterator<Vector<T>>;

    Vector()
    {
        allocate(2);
    }

    ~Vector()
    {
        clear();
        ::operator delete(m_array, sizeof(T) * m_capacity);
    }

    Vector(std::initializer_list<T> initList)
    {
        allocate(initList.size());
        for (const auto& item : initList)
            m_array[m_size++] = item;
    }

    Vector(const Vector& other)
    : m_size(other.m_size)
    , m_capacity(other.m_size)
    {
        allocate(m_capacity);
        for (size_t i = 0; i < m_size; ++i)
            m_array[i] = other.m_array[i];
    }

    void pushBack(const T& x)
    {
        if (m_size == m_capacity)
            allocate(m_capacity + m_capacity / 2);

        m_array[m_size++] = x;
    }

    void pushBack(T&& x)
    {
        if (m_size == m_capacity)
            allocate(m_capacity + m_capacity / 2);

        m_array[m_size++] = std::move(x);
    }

    template <typename ... ARGS>
    T& emplaceBack(ARGS&& ... args)
    {
        if (m_size == m_capacity)
            allocate(m_capacity + m_capacity / 2);

        new(&m_array[m_size]) T(std::forward<ARGS>(args)...);
        return m_array[m_size++];
    }

    void popBack()
    {
        if (m_size > 0)
        {
            m_size--;
            m_array[m_size].~T();
        }
    }

    size_t size() const
    {
        return m_size;
    }

    const T& operator[](size_t index) const
    {
        return m_array[index];
    }

    T& operator[](size_t index)
    {
        return m_array[index];
    }

    void clear()
    {
        for (size_t i = 0; i < m_size; ++i)
            m_array[i].~T();
        m_size = 0;
    }

    Iterator begin()
    {
        return Iterator(m_array);
    }

    Iterator end()
    {
        return Iterator(m_array + m_size);
    }

private:
    T* m_array {nullptr};
    size_t m_size {0};
    size_t m_capacity {0};

    void allocate(size_t newCapacity)
    {
        T* newArray = (T*) ::operator new(sizeof(T) * newCapacity);

        size_t old_size = m_size;
        if (newCapacity < m_size)
            m_size = newCapacity;

        for (size_t i = 0; i < m_size; ++i)
            newArray[i] = std::move(m_array[i]);

        for (size_t i = 0; i < old_size; ++i)
            m_array[i].~T();

        ::operator delete(m_array, sizeof(T) * m_capacity);

        m_array = newArray;
        m_capacity = newCapacity;
    }

};
