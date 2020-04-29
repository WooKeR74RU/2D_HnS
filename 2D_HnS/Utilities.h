#pragma once

bool doubleEqual(double a, double b);
bool doubleLess(double a, double b);

template<typename T>
T sqr(T val)
{
	return val * val;
}

void drawDot(double x, double y);
