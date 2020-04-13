#pragma once

struct Point;

struct Vector
{
	double x;
	double y;

	Vector() = default;
	Vector(double x, double y);
	Vector(const Point& a, const Point& b);
	explicit Vector(const Point& point);
	bool operator==(const Vector& other) const;
	bool operator!=(const Vector& other) const;
	Vector operator+(const Vector& other) const;
	Vector operator-(const Vector& other) const;
	Vector operator-() const;
	Vector operator*(double k) const;
	Vector operator/(double k) const;
	double getLength() const;
	void resize(double length);
	void rotate90();
};
