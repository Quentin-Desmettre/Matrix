# Matrix

Matrix is my attempt at making a highly optimized C++ matrix library.
I built it using only the C++ Standard Library.
It currently only uses the CPU.
Optimization were made by taking advantage of the CPU cache, and by using as many threads as possible.
In a future update, it will be possible to limit the maximum number of threads usable by the library.

## Installation

Use git to clone the repository.

```bash
git clone https://github.com/Quentin-Desmettre/Matrix.git
```

## Usage

```cpp
// Use the library
#include "Matrix/Matrix.hpp"

int main(void)
{
    // create a 3x2 matrix, containg only double values
    cppm::Matrix<double> mat1(3, 2);
    
    // create a 2x3 matrix, with all it's values being initialized at 1
    cppm::Matrix<double> mat2(2, 3, true, 1);
    
    // create a 3x3 matrix, resulting of the multiplication of mat1 by mat2
    cppm::Matrix<double> mat3 = mat1 * mat2;
    
    // Error: Incompatible types
    cppm::Matrix<int> mat4 = mat3;
    
    // Exception: Incompatibles sizes
    mat3 = cpmm::Matrix<double>(7, 7);
    mat3 *= mat2;
    
    return 0;
}
```
