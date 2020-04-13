#pragma once

struct Vector;

struct Point
{
	double x;
	double y;

	Point() = default;
	Point(double x, double y);
	explicit Point(const Vector& vector);

	bool operator==(const Point& other) const;
	bool operator!=(const Point& other) const;
};

struct PointCmpY
{
	bool operator()(Point* a, Point* b) const;
};
struct PointCmpX
{
	bool operator()(Point* a, Point* b) const;
};
