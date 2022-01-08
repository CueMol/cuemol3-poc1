#pragma once

#include <napi.h>

namespace node_jsbr {

class FloatArray
{
private:
    /// number of elements
    size_t m_nSize;

public:
    using value_type = float;

    FloatArray() : m_nSize(0) {}

    ///  Make array with size sz
    explicit FloatArray(size_t sz) : m_nSize(sz)
    {
        // m_array = MB_NEW _Type[sz];
    }

    ///  Make array with size sz and initialize all elements by ini
    explicit FloatArray(value_type ini_value, size_t sz) : m_nSize(sz)
    {
        // m_array = MB_NEW _Type[sz];
        // for (int i=0; i<sz; i++)
        //   m_array[i] = ini_value;
    }

    ///  Make array from existing C array
    explicit FloatArray(size_t sz, const value_type *p) : m_nSize(sz)
    {
        // m_array = MB_NEW _Type[sz];
        // for (int i=0; i<sz; i++)
        //   m_array[i] = p[i];
    }

    /// Copy constructor
    FloatArray(const FloatArray &arg) = delete;

    ~FloatArray() {}

    /////////////////////////////////////////////////////
    // member methods

    size_t size() const
    {
        return m_nSize;
    }

    void resize(size_t newsz)
    {
        // if (m_array != NULL) {
        //     delete[] m_array;
        //     m_array = NULL;
        //     m_nSize = 0;
        // }
        // if (newsz > 0) {
        //     m_array = MB_NEW _Type[newsz];
        //     m_nSize = newsz;
        // }
    }

    inline void allocate(size_t newsz)
    {
        resize(newsz);
    }
    inline void destroy()
    {
        resize(0);
    }

    const value_type &at(int i) const
    {
        MB_ASSERT(i >= 0);
        MB_ASSERT(i < m_nSize);
        // return m_array[i];
        return 0.0f;
    }

    value_type &at(int i)
    {
        MB_ASSERT(i >= 0);
        MB_ASSERT(i < m_nSize);
        // return m_array[i];
        return 0.0f;
    }

    const value_type *data() const
    {
        return nullptr;
        // return m_array;
    }

    value_type *data()
    {
        return nullptr;
        // return m_array;
    }

    /////////////////////////////////////////////////////
    // member operators

    operator const value_type *() const
    {
        return data();
    }

    const value_type &operator[](int i) const
    {
        return at(i);
    }

    value_type &operator[](int i)
    {
        return at(i);
    }
};

}  // namespace node_jsbr
