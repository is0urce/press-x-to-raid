// name: Point.h
// type: c++ header with implementation
// desc: 2-d point with integer components
// auth: is0urce

#pragma once

#include <functional>

namespace px
{
	struct Point
	{
	public:
		int X;
		int Y;

		Point() : X(0), Y(0) {}
		Point(int x, int y) : X(x), Y(y) {}
		Point(const Point& point) : X(point.X), Y(point.Y) {}

		Point& operator+=(const Point& rhs)
		{
			X += rhs.X;
			Y += rhs.Y;
			return *this;
		}
		Point& operator-=(const Point& rhs)
		{
			X -= rhs.X;
			Y -= rhs.Y;
			return *this;
		}
		void Write(int* memory) const { memory[0] = X; memory[1] = Y;}
		void Write(float* memory) const { memory[0] = (float)X; memory[1] = (float)Y; }
		void Write(double* memory) const { memory[0] = (double)X; memory[1] = (double)Y; }
		Point& Move(const Point& move) { return *this += move; }
		Point Moved(const Point& move) const { Point result = *this; return result += move; }
		Point Multiplied(int c) const { return Point(X * c, Y * c); }
		Point Multiplied(int w, int h) const { return Point(X * w, Y * h); }
		Point Clamped(const Point& min, const Point& max) const { return Point((std::min)((std::max)(min.X, X), max.X), (std::min)((std::max)(min.Y, Y), max.Y)); }
		int GetVolume() const { return X * Y; }
		bool InRange(const Point& start, const Point &range) const { return X >= start.X && Y >= start.Y && X < start.X + range.X && Y < start.Y + range.Y; }

		// distances
		int KingDistanceTo(const Point& p) const
		{
			return (std::max)(std::abs(X - p.X), std::abs(Y - p.Y));
		}
		int BlockDistanceTo(const Point& p) const
		{
			return std::abs(X - p.X) + std::abs(Y - p.Y);
		}
		double DistanceTo(const Point& p) const
		{
			return std::sqrt(std::pow(X - p.X, 2.0f) + std::pow(Y - p.Y, 2.0f));
		}
		double DistanceToSquared(const Point& p) const
		{
			int x = X - p.X;
			int y = Y - p.Y;
			return x * x + y * y;
		}

		// enumerators
		static void EnumerateRectangle(const Point& from, const Point &to, std::function<void(const Point&)> fun)
		{
			for (int j = from.Y; j < to.Y; j++)
			{
				for (int i = from.X; i < to.X; i++)
				{
					fun(Point(i, j));
				}
			}
		}
		static void EnumerateRectangle(Point to, std::function<void(const Point&)> fun)
		{
			EnumerateRectangle(Point(0, 0), to, fun);
		}
		void EnumerateRange(std::function<void(const Point&)> fun) const
		{
			EnumerateRectangle(Point(0, 0), *this, fun);
		}
		void EnumerateRange(const Point &from, std::function<void(const Point&)> fun) const
		{
			Point to = *this;
			to += from;
			EnumerateRectangle(from, to, fun);
		}

		static Point Zero() { return Point(0, 0); }
	};

	inline bool operator==(const Point& a, const Point& b) { return a.X == b.X && a.Y == b.Y; }
	inline bool operator!=(const Point& lhs, const Point& rhs) { return !operator==(lhs,rhs); }
	inline Point operator+(Point lhs, const Point& rhs) { lhs += rhs; return lhs; }
	inline Point operator-(Point lhs, const Point& rhs) { lhs -= rhs; return lhs; }
}
