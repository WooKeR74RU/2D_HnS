#pragma once

#include "Point.h"
#include "Vector.h"

double toDeg(double rad);
double toRad(double deg);

double distance(const Point& a, const Point& b);

double scalarProduct(const Vector& a, const Vector& b);
double vectorProduct(const Vector& a, const Vector& b);

double getSinBetween(const Vector& a, const Vector& b);
double getCosBetween(const Vector& a, const Vector& b);

double getAngle(const Vector& a, const Vector& b);

bool isPointOnLine(const Point& p1, const Point& p2, const Point& point);
