#ifndef CSTAR_BASE_HPP
#define CSTAR_BASE_HPP
#include <cassert>
#include <iostream>

#define cstar_assert(x) assert(x && false)
#define cstar_error(x) std::cerr << x << std::endl;

#endif