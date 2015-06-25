// name: Vector.h
// type: c++ header with implementation
// desc: 2-d point with real components
// auth: is0urce

#pragma once

#include "Point.h"

namespace px
{
	struct Vector
	{
	public:
		double X;
		double Y;

		Vector() : X(0.0), Y(0.0) {}
		Vector(double x, double y) : X(x), Y(y) {}
		Vector(const Vector& vec2) : X(vec2.X), Y(vec2.Y) {}
		Vector(const Point& point) : X(point.X), Y(point.Y) {}

		Vector& operator+=(const Vector& rhs)
		{
			X += rhs.X;
			Y += rhs.Y;
			return *this;
		}
		Vector& operator-=(const Vector& rhs)
		{
			X -= rhs.X;
			Y -= rhs.Y;
			return *this;
		}
		void Write(float* memory) const { memory[0] = (float)X; memory[1] = (float)Y; };
		void Write(double* memory) const { memory[0] = X; memory[1] = Y; };
		Vector& Move(const Vector& move) { return *this += move; }
		Vector Moved(const Vector& move) const { Vector result = *this; return result += move; };
		Vector Clamped(const Vector& min, const Vector& max) const { return Vector((std::min)((std::max)(min.X, X), max.X), (std::min)((std::max)(min.Y, Y), max.Y)); }

		static Vector Zero() { return Vector(0.0, 0.0); }
	};

	inline bool operator==(const Vector& a, const Vector& b) { return a.X == b.X && a.Y == b.Y; }
	inline bool operator!=(const Vector& lhs, const Vector& rhs) { return !operator==(lhs,rhs); }
	inline Vector operator+(Vector lhs, const Vector& rhs) { lhs += rhs; return lhs; }
	inline Vector operator-(Vector lhs, const Vector& rhs) { lhs -= rhs; return lhs; }
}
