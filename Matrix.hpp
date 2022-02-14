#ifndef E3096311_CEA5_475E_847E_7C8C54043D10
#define E3096311_CEA5_475E_847E_7C8C54043D10

#include <utility>
#include <string>
#include <cstring>

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
            _size[0] = other->_size[0];
            _size[1] = other->_size[1];
            _size[2] = other->_size[2];

            std::memcpy(_elems, other->_elems, sizeof(Type) * _size[2]);
        }
        Matrix<Type> _addPtr(Matrix<Type> const *other)
        {
            if (other->_size[0] != _size[0] || other->_size[1] != _size[1])
                throw "Incompatible sizes";
            Matrix<Type> r(other);
            for (uint64 i = 0, n = _size[2]; i < n; i++)
                r._elems[i] += _elems[i];
            return r;
        }
        Matrix<Type> _minusPtr(Matrix<Type> const *other)
        {
            if (other->_size[0] != _size[0] || other->_size[1] != _size[1])
                throw "Incompatible sizes";
            Matrix<Type> r(other);
            for (uint64 i = 0, n = _size[2]; i < n; i++)
                r._elems[i] -= _elems[i];
            return r;
        }
        Matrix<Type> _mulPtr(Matrix<Type> const *other)
        {
            if (_size[1] != other->_size[0])
                throw "Incompatible sizes";
            Matrix<Type> result(_size[0], other->_size[1], true);

            uint64 resPos;
            uint64 resPos1;

            uint64 myPos;

            for(uint64 i = 0, rowX = _size[0]; i < rowX; ++i) {
                resPos1 = i * result._size[0];
                for(uint64 j = 0, colY = other->_size[1]; j < colY; ++j) {
                    resPos = resPos1 + j;
                    myPos = i * _size[0];
                    for(uint64 k = 0, rowY = other->_size[0]; k < rowY; ++k) {
                        result._elems[resPos] += _elems[myPos] * other->at(k, j);
                        //result.at(i, j) += at(i, k) * other->at(k, j);
                    }
                }
            }
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
                for (uint64 i = 0, n = _size[2]; i < n; i++)
                    _elems[i] = filler;
        }
        Matrix(Matrix<Type> const& other)
        {
            _copyPtr(&other);
        }
        Matrix(Matrix<Type> const *other)
        {
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
        const size_t& getSize(void) {return _size;}
        Type &at(uint64 const i, uint64 const j)
        {
            if (j > _size[1])
                throw "Bad column index: " + std::to_string(j);
            if (i > _size[0])
                throw "Bad row index: " + std::to_string(i);
            return _elems[i * _size[0] + j];
        }
    };
}

/* operators:

Todo:

*: Type2
/: Type2

+=: Matrix
-=: Matrix
*=: Matrix, Type2
/=: Type2

==: Matrix
!=: Matrix

Statics for identity

*/

#endif // E3096311_CEA5_475E_847E_7C8C54043D10
