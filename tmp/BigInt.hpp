#ifndef E0801298_A90F_4B40_BAC4_7BA28C80893D
#define E0801298_A90F_4B40_BAC4_7BA28C80893D

#include <vector>

typedef unsigned long uint64;
typedef unsigned long int64;

class BigInt
{
private:
    std::vector<uint64> _bits;
public:
    BigInt(int64 const& base = 0);
    BigInt(uint64 const& base = 0);
    ~BigInt();
};

#endif // E0801298_A90F_4B40_BAC4_7BA28C80893D
