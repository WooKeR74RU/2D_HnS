#pragma once

struct Point;

struct Vector
{
	double x;
	double y;

	static const Vector Zero;

	Vector();
	Vector(double x, double y);
	Vector(const Point& a, const Point& b);
	explicit Vector(const Point& point);
	bool operator==(const Vector& other) const;
	bool operator!=(const Vector& other) const;
	Vector operator-() const;
	Vector operator+(const Vector& other) const;
	void operator+=(const Vector& other);
	Vector operator-(const Vector& other) const;
	void operator-=(const Vector& other);
	Vector operator*(double k) const;
	void operator*=(double k);
	Vector operator/(double k) const;
	void operator/=(double k);
	double getLength() const;
	void resize(double length);
	void increase(double value);
	void decrease(double value);
	void rotate90();
};
