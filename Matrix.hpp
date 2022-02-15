#ifndef E3096311_CEA5_475E_847E_7C8C54043D10
#define E3096311_CEA5_475E_847E_7C8C54043D10

#include <utility>
#include <string>
#include <cstring>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

typedef unsigned long uint64;

template<class Type>
void _mulPtr_n(uint64 i0, uint64 const iMax, uint64 const kMax, uint64 const jMax, uint64 const offset,
    Type *relm, Type const *oelm, Type const *elm)
{
    for(; i0 < iMax; ++i0)
        for(uint64 k = 0; k < kMax; ++k)
            for(uint64 j = 0; j < jMax; ++j)
                relm[i0 * jMax + j] += elm[i0 * offset + k] * oelm[k * jMax + j];
}

template<class Type>
void _addPtr_n(uint64 i0, uint64 const iMax, Type *relm, Type const *elm)
{
    for (; i0 < iMax; ++i0)
        relm[i0] += elm[i0];
}

template<class Type>
void _subPtr_n(uint64 i0, uint64 const iMax, Type *relm, Type const *elm)
{
    for (; i0 < iMax; ++i0)
        relm[i0] -= elm[i0];
}

template<class Type, class T2>
void _mulPtr_n2(uint64 i0, uint64 const iMax, Type *relm, T2 const elm)
{
    for (; i0 < iMax; ++i0)
        relm[i0] *= elm;
}

template<class Type, class T2>
void _divPtr_n(uint64 i0, uint64 const iMax, Type *relm, T2 const elm)
{
    for (; i0 < iMax; ++i0)
        relm[i0] /= elm;
}

namespace cppm
{
    typedef unsigned long uint64;
    typedef long int64;
    typedef uint64 size_t[3];

    static const unsigned int __MAX_THREADS = std::thread::hardware_concurrency() / 2;

    template <class Type>
    class Matrix
    {
    private:
        Type *_elems;
        size_t _size;
        std::vector<uint64> _segmentsMul;
        std::vector<uint64> _segmentsLine;

        void _initSegmentWith(std::vector<uint64> &s, uint64 const& max)
        {
            if (__MAX_THREADS == 0)
                return;
            s.reserve(__MAX_THREADS * 2);

            uint64 start = 0;
            uint64 end = max / __MAX_THREADS;
            uint64 const offset = end;
            unsigned int i = 0;
            unsigned const n = (__MAX_THREADS - 1) * 2;

            for (; i < n; i += 2) {
                s[i] = start;
                s[i + 1] = end;
                start += offset;
                end += offset;
            }
            s[i] = start;
            s[i + 1] = max;
        }
        void _initSegments(void)
        {
            _initSegmentWith(_segmentsMul, _size[0]);
            _initSegmentWith(_segmentsLine, _size[2]);
        }
        void _copyPtr(Matrix<Type> const *other)
        {
            _size[0] = other->_size[0];
            _size[1] = other->_size[1];
            _size[2] = other->_size[2];

            _initSegments();

            std::memcpy(_elems, other->_elems, sizeof(Type) * _size[2]);
        }
        Matrix<Type> _addPtr(Matrix<Type> const *other) const
        {
            if (other->_size[0] != _size[0] || other->_size[1] != _size[1])
                throw "Incompatible sizes";
            Matrix<Type> r(other);

            uint64 const n = _size[2];
            Type const *elm = _elems;
            Type *relm = r._elems;

            if (n >= __MAX_THREADS) {
                std::thread threads[__MAX_THREADS];

                for (int i = 0; i < __MAX_THREADS; ++i)
                    threads[i] = std::thread(_addPtr_n<Type>, _segmentsLine[2 * i], _segmentsLine[2 * i + 1],
                                             relm, elm);
                for (int i = 0; i < __MAX_THREADS; ++i)
                    threads[i].join();

            } else {
                for (uint64 i = 0; i < n; ++i)
                    relm[i] += elm[i];
            }
            return r;
        }
        Matrix<Type> _minusPtr(Matrix<Type> const *other) const
        {
            if (other->_size[0] != _size[0] || other->_size[1] != _size[1])
                throw "Incompatible sizes";
            Matrix<Type> r(other);

            uint64 const n = _size[2];
            Type const *elm = _elems;
            Type *relm = r._elems;

            if (n >= __MAX_THREADS) {
                std::thread threads[__MAX_THREADS];

                for (int i = 0; i < __MAX_THREADS; ++i)
                    threads[i] = std::thread(_subPtr_n<Type>, _segmentsLine[2 * i], _segmentsLine[2 * i + 1],
                                             relm, elm);
                for (int i = 0; i < __MAX_THREADS; ++i)
                    threads[i].join();

            } else {
                for (uint64 i = 0; i < n; ++i)
                    relm[i] -= elm[i];
            }
            return r;
        }
        Matrix<Type> _mulPtr(Matrix<Type> const *other) const
        {
            if (_size[1] != other->_size[0])
                throw "Incompatible sizes";

            uint64 const kMax = other->_size[0];
            uint64 const iMax = _size[0];
            uint64 const offset = _size[1];
            uint64 const jMax = other->_size[1];
            Matrix<Type> result(iMax, jMax, true);

            Type *relm = result._elems;
            Type const *oelm = other->_elems;
            Type const *elm = _elems;

            if (iMax >= __MAX_THREADS) {
                std::thread threads[__MAX_THREADS];

                for (unsigned i = 0; i < __MAX_THREADS; ++i)
                    threads[i] = std::thread(_mulPtr_n<Type>, _segmentsMul[2 * i], _segmentsMul[2 * i + 1], kMax, jMax, offset, relm, oelm, elm);
                for (unsigned i = 0; i < __MAX_THREADS; ++i)
                    threads[i].join();

            } else {
                for(uint64 i = 0; i < iMax; ++i)
                    for(uint64 k = 0; k < kMax; ++k)
                        for(uint64 j = 0; j < jMax; ++j)
                            relm[i * jMax + j] +=
                                elm[i * offset + k] * oelm[k * jMax + j];
            }
            return result;
        }
        template <class T2>
        Matrix<Type> _mulConst(T2 const &other) const
        {
            Matrix<Type> r(this);

            uint64 const n = _size[2];
            Type *elm = r._elems;

            if (n >= __MAX_THREADS) {
                std::thread threads[__MAX_THREADS];

                for (int i = 0; i < __MAX_THREADS; ++i)
                    threads[i] = std::thread(_mulPtr_n2<Type, T2>, _segmentsLine[2 * i], _segmentsLine[2 * i + 1],
                                             elm, other);
                for (int i = 0; i < __MAX_THREADS; ++i)
                    threads[i].join();

            } else {
                for (uint64 i = 0; i < n; ++i)
                    elm[i] *= other;
            }
            return r;
        }
        template <class T2>
        Matrix<Type> _divConst(T2 const &other) const
        {
            Matrix<Type> r(this);

            uint64 const n = _size[2];
            Type *elm = r._elems;

            if (n >= __MAX_THREADS) {
                std::thread threads[__MAX_THREADS];

                for (int i = 0; i < __MAX_THREADS; ++i)
                    threads[i] = std::thread(_divPtr_n<Type, T2>, _segmentsLine[2 * i], _segmentsLine[2 * i + 1],
                                             elm, other);
                for (int i = 0; i < __MAX_THREADS; ++i)
                    threads[i].join();

            } else {
                for (uint64 i = 0; i < n; ++i)
                    elm[i] /= other;
            }
            return r;
        }
    public:
        Matrix(uint64 const nb_line = 1, uint64 const nb_col = 1, bool const fill = false, Type const filler = Type())
        {
            if (!nb_line || !nb_col)
                throw "Invalid null size";
            _size[0] = nb_line;
            _size[1] = nb_col;
            _size[2] = nb_col * nb_line;

            _initSegments();

            _elems = new Type[_size[2]];
            if (fill)
                for (uint64 i = 0; i < _size[2]; ++i)
                    _elems[i] = filler;
        }
        Matrix(Matrix<Type> const& other)
        {
            _elems = new Type[other._size[2]];
            _copyPtr(&other);
        }
        Matrix(Matrix<Type> const *other)
        {
            _elems = new Type[other->_size[2]];
            _copyPtr(other);
        }
        ~Matrix()
        {
            delete [] _elems;
        }

        Matrix<Type>& operator=(Matrix<Type> const& other)
        {
            if (_size[2] != other._size[2]) {
                delete [] _elems;
                _elems = new Type[other._size[2]];
            }
            _copyPtr(&other);
            return *this;
        }
        Matrix<Type>& operator=(Matrix<Type> const *other)
        {
            if (_size[2] != other->_size[2]) {
                delete [] _elems;
                _elems = new Type[other->_size[2]];
            }
            _copyPtr(other);
            return *this;
        }

        Matrix<Type> operator+(Matrix<Type> const &other) const
        {
            return _addPtr(&other);
        }
        Matrix<Type> operator+(Matrix<Type> const *other) const
        {
            return _addPtr(other);
        }

        Matrix<Type> operator-(Matrix<Type> const &other) const
        {
            _minusPtr(&other);
        }
        Matrix<Type> operator-(Matrix<Type> const *other) const
        {
            _minusPtr(other);
        }

        Matrix<Type> operator*(Matrix<Type> const &other) const
        {
            return _mulPtr(&other);
        }
        Matrix<Type> operator*(Matrix<Type> const *other) const
        {
            return _mulPtr(other);
        }

        template<class T2>
        Matrix<Type> operator*(T2 const& other) const
        {
            return _mulConst<T2>(other);
        }
        template<class T2>
        Matrix<Type> operator/(T2 const& other) const
        {
            return _divConst<T2>(other);
        }

        template <class T2>
        friend Matrix<Type> operator*(Matrix<Type> const& a, T2 const &other)
        {
            return a.operator*(other);
        }
        const size_t& getSize() const {return _size;}
        Type &at(uint64 const i, uint64 const j) const
        {
            if (i >= _size[0])
                throw "Bad row index: " + std::to_string(i);
            if (j >= _size[1])
                throw "Bad column index: " + std::to_string(j);
            return _elems[i * _size[1] + j];
        }
    };
}

/* operators:

Todo:

+=: Matrix
-=: Matrix
*=: Matrix, Type2
/=: Type2

==: Matrix
!=: Matrix

Statics for identity

*/

#endif // E3096311_CEA5_475E_847E_7C8C54043D10
