#ifndef E3096311_CEA5_475E_847E_7C8C54043D10
#define E3096311_CEA5_475E_847E_7C8C54043D10

#include <utility>
#include <string>
#include <cstring>
#include <chrono>
#include <iostream>
namespace cppm
{
    typedef unsigned long uint64;
    typedef long int64;
    typedef uint64 size_t[3];

    template <class Type>
    class Matrix
    {
    private:
        Type *_elems;
        size_t _size;

        void _copyPtr(Matrix<Type> const *other)
        {
            size_t const os = other->_size;

            _size[0] = os[0];
            _size[1] = os[1];
            _size[2] = os[2];

            std::memcpy(_elems, other->_elems, sizeof(Type) * _size[2]);
        }
        Matrix<Type> _addPtr(Matrix<Type> const *other)
        {
            if (other->_size[0] != _size[0] || other->_size[1] != _size[1])
                throw "Incompatible sizes";
            Matrix<Type> r(other);

            uint64 const n = _size[2];
            Type const *elm = _elems;

            for (uint64 i = 0; i < n; i++)
                r._elems[i] += elm[i];
            return r;
        }
        Matrix<Type> _minusPtr(Matrix<Type> const *other)
        {
            if (other->_size[0] != _size[0] || other->_size[1] != _size[1])
                throw "Incompatible sizes";
            Matrix<Type> r(other);

            uint64 const n = _size[2];
            Type const *elm = _elems;

            for (uint64 i = 0; i < n; i++)
                r._elems[i] -= elm[i];
            return r;
        }
        Matrix<Type> _mulPtr(Matrix<Type> const *other)
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

            for(uint64 i = 0; i < iMax; ++i)
                for(uint64 k = 0; k < kMax; ++k)
                    for(uint64 j = 0; j < jMax; ++j)
                        relm[i * jMax + j] +=
                            elm[i * offset + k] * oelm[k * jMax + j];
            return result;
        }
    public:
        Matrix(uint64 const nb_line = 1, uint64 const nb_col = 1, bool const fill = false, Type const filler = Type())
        {
            if (!nb_line || !nb_col)
                throw "Invalid null size";
            _size[0] = nb_line;
            _size[1] = nb_col;
            _size[2] = nb_col * nb_line;

            _elems = new Type[_size[2]];
            if (fill)
                for (uint64 i = 0; i < _size[2]; i++)
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

        Matrix<Type> operator+(Matrix<Type> const &other)
        {
            return _addPtr(&other);
        }
        Matrix<Type> operator+(Matrix<Type> const *other)
        {
            return _addPtr(other);
        }

        Matrix<Type> operator-(Matrix<Type> const &other)
        {
            _minusPtr(&other);
        }
        Matrix<Type> operator-(Matrix<Type> const *other)
        {
            _minusPtr(other);
        }

        Matrix<Type> operator*(Matrix<Type> const &other)
        {
            return _mulPtr(&other);
        }
        Matrix<Type> operator*(Matrix<Type> const *other)
        {
            return _mulPtr(other);
        }

        template<class T2>
        Matrix<Type> operator*(T2 const& other)
        {
            Matrix<Type> r(this);
            Type *elm = r._elems;

            for (int i = 0; i < _size[2]; i++)
                elm[i] *= other;
            return r;
        }
        template<class T2>
        Matrix<Type> operator/(T2 const& other)
        {
            Matrix<Type> r(this);
            Type *elm = r._elems;
            uint64 const size = _size[2];

            for (uint64 i = 0; i < size; i++)
                elm[i] /= other;
            return r;
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
