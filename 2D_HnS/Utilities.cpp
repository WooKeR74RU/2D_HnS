#include "Utilities.h"

#include <cmath>

const double EPS = 1e-6;
bool doubleEqual(double a, double b)
{
	return std::abs(a - b) < EPS;
}
bool doubleLess(double a, double b)
{
	return !doubleEqual(a, b) && a < b;
}
