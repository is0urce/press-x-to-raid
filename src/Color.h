// name: Color.h
// type: c++ header with implementation
// desc: color with double precise float rgb components
// auth: is0urce

#pragma once

#include <cmath>

namespace px
{
	// hide constants
	namespace
	{
		const double PI = 3.141592653589793238463; // more than enought
		const char* HEX = "0123456789ABCDEF";
	}

	struct Color
	{
		// attributes
	public:
		double R;
		double G;
		double B;
		double A;

		// ctor & dtor
	public:
		Color(): R(0), G(0), B(0), A(0) {};
		Color(double r, double g, double b): R(r), G(g), B(b), A(1) {};
		Color(double r, double g, double b, double a): R(r), G(g), B(b), A(a) {};
		Color(int code) { SetHex(code); }

		// methods
	public:
		static Color FromRGB(int r, int g, int b) { return Color(r / 256.0, g / 256.0, b / 256.0); }
		static Color FromLuminance(double luminance) { return Color(luminance, luminance, luminance); }
		static Color Black() { return Color(0, 0, 0, 1); }; 
		static Color White() { return Color(1, 1, 1, 1); };
		static Color Transparent() { return Color(0, 0, 0, 0); };

		void Set(const Color& color) { R = color.R; G = color.G; B = color.B; A = color.A; }
		void Set(double r, double g, double b, double a) { R = r; G = g; B = b; A = a; };
		void Set(double r, double g, double b) { R = r; G = g; B = b; A = 1.0; }
		void SetRGBA(int r, int g, int b, int a) { R = r / 255.0; G = g / 255.0; B = b / 255.0; A = a / 255.0; }
		void SetRGB(int r, int g, int b) { R = r / 255.0; G = g / 255.0; B = b / 255.0; A = 1.0; }
		void SetHex(int code) { SetRGB(code / 256 / 256 % 256, code / 256 % 256, code % 256); };
		void Write(double* memory) const { memory[0] = R; memory[1] = G; memory[2] = B; memory[3] = A; };
		void Write(float* memory) const { memory[0] = (float)R; memory[1] = (float)G; memory[2] = (float)B; memory[3] = (float)A; };
		void ShiftHue(double angle) { *this = TransformHue(*this, angle); };
		void ShiftHSV(double H, double S, double V) { *this = TransformHSV(*this, H, S, V); };
		Color TransformHSV(double H, double S, double V) const { return TransformHSV(*this, H, S, V); };
		Color Average(const Color& other) const { return (*this + other) / 2; };
		double Luminance() const { return 0.2125 * R + 0.7154 * G + 0.0721 * B; };

		// H - hue shift (in degrees) S - saturation multiplier (scalar), V value multiplier (scalar)
		static Color TransformHSV(const Color& in, double H, double S, double V)
		{
			double VSU = V * S * std::cos(H * PI / 180.0);
			double VSW = V * S * std::sin(H * PI / 180.0);

			Color ret(in);
			ret.R = (.299*V+.701*VSU+.168*VSW)*in.R
				+ (.587*V-.587*VSU+.330*VSW)*in.G
				+ (.114*V-.114*VSU-.497*VSW)*in.B;
			ret.G = (.299*V-.299*VSU-.328*VSW)*in.R
				+ (.587*V+.413*VSU+.035*VSW)*in.G
				+ (.114*V-.114*VSU+.292*VSW)*in.B;
			ret.B = (.299*V-.3*VSU+1.25*VSW)*in.R
				+ (.587*V-.588*VSU-1.05*VSW)*in.G
				+ (.114*V+.886*VSU-.203*VSW)*in.B;

			return ret;
		}
		static Color TransformHue(const Color& in, double angle) 
		{
			double U = std::cos(angle * PI / 180.0);
			double W = std::sin(angle * PI / 180.0);

			Color ret(in);
			ret.R = (.299 + .701*U+.168*W) * in.R
				+ (.587 -.587*U+.330*W) * in.G
				+ (.114 -.114*U-.497*W) * in.B;
			ret.G = (.299 -.299*U-.328*W) * in.R
				+ (.587 + .413*U+.035*W) * in.G
				+ (.114 -.114*U+.292*W) * in.B;
			ret.B = (.299 -.3*U+1.25*W) * in.R
				+ (.587 -.588*U-1.05*W) * in.G
				+ (.114 + .886*U-.203*W) * in.B;

			return ret;
		}
		static Color Average(const Color& a, const Color& b) { return (a + b) / 2; };
		static Color Average(const Color& a, const Color& b, const Color& c) { return (a + b + c) / 3; };
		static Color Average(const Color& a, const Color& b, const Color& c, const Color& d) { return (a + b + c + d) / 4; };

		// operators (all as members due to heavy use in inlined methods)
		Color operator-() const { return Color(-R, -G, -B); };
		Color& operator+=(const Color& color) { R += color.R; G += color.G; B += color.B; A += color.A; return *this; };
		Color& operator-=(const Color& color) { R -= color.R; G -= color.G; B -= color.B; A -= color.A; return *this; };
		Color& operator*=(const Color& color) { R *= color.R, G *= color.G, B *= color.B, A *= color.A; return *this; };

		bool operator==(const Color& color) const { return R == color.R && G == color.G && B == color.B; }
		bool operator!=(const Color& color) const { return !(*this == color); }
		Color operator+(const Color& color) const { Color result = *this; return result += color; }
		Color operator-(const Color& color) const { Color result = *this; return result -= color; }
		Color operator*(const Color& color) const { Color result = *this; return result *= color; }

		Color operator*(double c) const { return Color(R * c, G * c, B * c, A * c); };
		Color operator/(double c) const { return Color(R / c, G / c, B / c, A / c); };
		Color operator*=(double c) { *this = *this * c; return *this; };
		Color operator/=(double c) { *this = *this / c; return *this; };
	};
}