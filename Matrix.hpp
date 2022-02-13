#ifndef MATRIX_HPP
    #define MATRIX_HPP
    #include <utility>
    #include <cstdlib>
    #include <stdexcept>
    #include <string>
    #include <type_traits>
    #include <thread>
    #include <cstring>

namespace cppm
{
    typedef unsigned long uint64;
    typedef long int64;
    typedef std::pair<uint64, uint64> m_size_t;
    typedef enum {ROW, COLUMN, NORMAL} matrix_type;
    static const unsigned int NB_THREADS = std::thread::hardware_concurrency();
    static const uint64 SEGMENT_SIZE = NB_THREADS * 2;

    uint64 *segment_threads(uint64 const size)
    {
        uint64 *segment = (uint64 *)malloc(sizeof(uint64) * SEGMENT_SIZE);
        const uint64 step = size / NB_THREADS;
        uint64 start = 0;
        uint64 end = step;
        int i = 0;

        for (; i < SEGMENT_SIZE - 2; i += 2) {
            segment[i] = start;
            segment[i + 1] = end;
            start += step;
            end += step;
        }
        segment[i] = start;
        segment[i + 1] = size;
        return segment;
    }

    class IncompatibleSizeException : public std::exception
    {
    private:
        std::string m_what;
    public:
        IncompatibleSizeException(m_size_t const& a, m_size_t const&b)
        {
            m_what = "Incompatible sizes: ";
            m_what += "(" + std::to_string(a.first) + ";" + std::to_string(a.second) + ")";
            m_what += " and ";
            m_what += "(" + std::to_string(a.first) + ";" + std::to_string(a.second) + ")";
        }
        const char *what()
        {
            return m_what.c_str();
        }
    };

    template <class ElemType>
    class RowMatrix
    {
    private:
        ElemType *m_elems;
        uint64 m_size;

    public:
        RowMatrix(uint64 const& size, bool fill = false, ElemType const filler = ElemType())
        {
            m_size = size;
            m_elems = (ElemType *)malloc(sizeof(ElemType) * m_size);

            if (!m_elems)
                throw std::bad_alloc();
            if (fill)
                for (uint64 i = 0; i < size; i++)
                    m_elems[i] = filler;
        }
        RowMatrix(RowMatrix<ElemType> const& origin)
        {
            m_size = origin.m_size;
            m_elems = (ElemType *)malloc(sizeof(ElemType) * m_size);

            memcpy(m_elems, origin.m_elems, m_size * sizeof(ElemType));
        }
        RowMatrix(RowMatrix<ElemType> const *origin)
        {
            m_size = origin->m_size;
            m_elems = (ElemType *)malloc(sizeof(ElemType) * m_size);

            memcpy(m_elems, origin->m_elems, m_size * sizeof(ElemType));
        }
        RowMatrix(const ElemType array[])
        {
            m_size = sizeof(array) / sizeof(ElemType);
            m_elems = (ElemType *)malloc(sizeof(ElemType) * m_size);

            memcpy(m_elems, array, m_size * sizeof(ElemType));
        }
        RowMatrix(const ElemType *array, const uint64 &size)
        {
            m_size = size;
            m_elems = (ElemType *)malloc(sizeof(ElemType) * m_size);

            memcpy(m_elems, array, m_size * sizeof(ElemType));
        }

        uint64 size(void) const {return m_size;}

        ElemType& operator = (const RowMatrix<ElemType> &other)
        {
            m_size = other.m_size;
            m_elems = (ElemType *)realloc(m_elems, m_size);

            memcpy(m_elems, other.m_elems, m_size * sizeof(ElemType));
            return *this;
        }
        ElemType& operator = (const ElemType other[])
        {
            m_size = sizeof(other) / sizeof(ElemType);
            m_elems = (ElemType *)realloc(m_elems, m_size);

            memcpy(m_elems, other, m_size * sizeof(ElemType));
            return *this;
        }

        ElemType& operator [] (uint64 const& i) const
        {
            if (i >= m_size)
                throw std::out_of_range("Index too big: " + std::to_string(i) + ".");
            return m_elems[i];
        }

        RowMatrix<ElemType> operator + (RowMatrix<ElemType> const& a) const
        {
            if (a.m_size != m_size)
                throw cppm::IncompatibleSizeException(m_size_t(1, m_size), m_size_t(1, a.m_size));
            RowMatrix<ElemType> result(a);

            for (int i = 0; i < m_size; i++)
                result.m_elems[i] += m_elems[i];
            return result;
        }
        template <class T>
        RowMatrix<ElemType> operator + (T const& n) const
        {
            if (m_size != 1)
                throw cppm::IncompatibleSizeException(m_size_t(1, m_size), m_size_t(1, 1));
            RowMatrix<ElemType> result(this);

            result.m_elems[0] += n;

            return result;
        }
        RowMatrix<ElemType> operator + (void) const
        {
            return RowMatrix<ElemType>(this);
        }

        RowMatrix<ElemType> operator - (RowMatrix<ElemType> const& a) const
        {
            if (a.m_size != m_size)
                throw cppm::IncompatibleSizeException(m_size_t(1, m_size), m_size_t(1, a.m_size));
            RowMatrix<ElemType> result(this);

            for (int i = 0; i < m_size; i++)
                result.m_elems[i] -= a[i];
            return result;
        }
        template <class T>
        RowMatrix<ElemType> operator - (T const& n) const
        {
            if (m_size != 1)
                throw cppm::IncompatibleSizeException(m_size_t(1, m_size), m_size_t(1, 1));
            RowMatrix<ElemType> result(this);

            result.m_elems[0] -= n;

            return result;
        }
        RowMatrix<ElemType> operator - (void) const
        {
            RowMatrix<ElemType> result(this);

            for (int i = 0; i < m_size; i++)
                result.m_elems[i] *= -1;
            return result;
        }

        template <class T>
        RowMatrix<ElemType> operator * (T const& n) const
        {
            RowMatrix<ElemType> result(this);

            for (int i = 0; i < m_size; i++)
                result.m_elems[i] *= n;
            return result;
        }

        RowMatrix<ElemType> &operator += (RowMatrix<ElemType> const& a) const
        {
            if (a.m_size != m_size)
                throw cppm::IncompatibleSizeException(m_size_t(1, m_size), m_size_t(1, a.m_size));
            for (int i = 0; i < m_size; i++)
                m_elems[i] += a.m_elems[i];
            return *this;
        }
        RowMatrix<ElemType> &operator += (ElemType const& a) const
        {
            if (m_size != 1)
                throw cppm::IncompatibleSizeException(m_size_t(1, m_size), m_size_t(1, 1));
            m_elems[0] += a;
            return *this;
        }
        RowMatrix<ElemType> &operator -= (RowMatrix<ElemType> const& a) const
        {
            if (a.m_size != m_size)
                throw cppm::IncompatibleSizeException(m_size_t(1, m_size), m_size_t(1, a.m_size));
            for (int i = 0; i < m_size; i++)
                m_elems[i] -= a.m_elems[i];
            return *this;
        }
        RowMatrix<ElemType> &operator -= (ElemType const& a) const
        {
            if (m_size != 1)
                throw cppm::IncompatibleSizeException(m_size_t(1, m_size), m_size_t(1, 1));
            m_elems[0] -= a;
            return *this;
        }

        ~RowMatrix()
        {
            free(m_elems);
        }
    };
}

template <class T1, class T2>
cppm::RowMatrix<T1> operator + (T2 const a, cppm::RowMatrix<T1> const& b)
{
    return b + a;
}
template <class T1, class T2>
cppm::RowMatrix<T1> operator * (T2 const a, cppm::RowMatrix<T1> const& b)
{
    return b * a;
}

#endif
