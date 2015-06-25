// name: Shadow.h
// desc: Shadowcasting
// type: c++ header & implementation
// auth: is0urce
// date: 2014-12-16

#pragma once

#include "Scene.h"

namespace px
{
	// octants
	static int multipliers[4][8] =
	{
		{1, 0, 0, -1, -1, 0, 0, 1},
		{0, 1, -1, 0, 0, -1, 1, 0},
		{0, 1, 1, 0, 0, -1, -1, 0},
		{1, 0, 0, 1, -1, 0, 0, -1}
	};

	class Shadow
	{
	private:
		std::unique_ptr<bool[]> _fov;
		int _radius;
		int _width;
		int _len;
		Point _offset;

	public:
		Shadow(int radius)
		{
			Radius(radius);
		}
		Shadow(const vr::Scene& map, const Point& start, int radius)
		{
			Radius(radius);
			Cast(map, start, radius);
		}
		~Shadow()
		{
		}

		void Radius(int radius)
		{
			if (radius < 0) throw std::runtime_error("Shadow::Radius(...) radius < 0");

			_radius = radius;
			_width = _radius * 2 + 1;
			_len = _width * _width;
			_fov = std::unique_ptr<bool[]>(new bool[_len]);
		}
		void Clear()
		{
			for (int i = 0; i < _len; i++)
			{
				_fov[i] = false;
			}
		}
		void Light(Point point)
		{
			point += Point(_radius, _radius) - _offset;

			int pos = point.Y * _width + point.X;
			if (pos < 0 || pos >= _len)
			{
				throw std::runtime_error("Shadow::Light(...) out of range");
			}

			_fov[pos] = true;
		}
		bool IsInFOV(Point position)
		{
			position += Point(_radius, _radius) - _offset;

			if (position.X < 0 || position.Y < 0 || position.X >= _width || position.Y >= _width) return false;

			return _fov[position.Y * _width + position.X];
		}

		void Cast(const vr::Scene& map, const Point& start)
		{
			Cast(map, start, _radius);
		}
		void Cast(const vr::Scene& map, const Point& start, int radius)
		{
			if (radius < 0) throw std::runtime_error("Shadow::Cast(...) radius < 0");
			if (radius > _radius)
			{
				Radius(radius);
			}

			Clear();
			_offset = start;
			Light(start);
			for (int i = 0; i < 8; i++)
			{
				Cast(map, start.X, start.Y, radius, 1, 1.0, 0.0, multipliers[0][i], multipliers[1][i], multipliers[2][i], multipliers[3][i]);
			}
		}

	private:
		void Cast(const vr::Scene& map, int x, int y, int radius, int row, float start, float end, int xx, int xy, int yx, int yy)
		{
			if (start < end) return;
			int radius2 = radius * radius;

			float next_start_slope = start;
			for (int i = row; i <= radius; i++)
			{
				int dx = -i - 1;
				int dy = -i;
				bool blocked = false;
				while (dx <= 0)
				{
					dx++;
					float l_slope = (dx - 0.5f) / (dy + 0.5f);
					float r_slope = (dx + 0.5f) / (dy - 0.5f);

					if (start < r_slope)
					{
						continue;
					}
					else if (end > l_slope)
					{
						break;
					}
					else
					{
						Point absolute(x + dx * xx + dy * xy, y + dx * yx + dy * yy);

						// to avoid behind-collumn peek you have to see center of a tile to actualy see it (or it shoult be wall)
						float slope = (float)(dx) / (float)(dy);
						bool wall = !map.IsTransparent(absolute);
						bool center = start >= slope && slope >= end; 
						if ((center || wall) && (dx * dx + dy * dy < radius2))
						{
							Light(absolute);
						}

						if (blocked)
						{
							if (wall)
							{
								next_start_slope = r_slope;
								continue;
							}
							else
							{
								blocked = false;
								start = next_start_slope;
							}
						}
						else 
						{
							if (wall && i < radius)
							{
								blocked = true;
								Cast(map, x, y, radius, i + 1, start, l_slope, xx, xy, yx, yy);
								next_start_slope = r_slope;
							}
						}
					}
				}
				if (blocked) break;
			}
		}
	};
}

//typedef unsigned int uint;
//void cast_light(Scene& scene, uint x, uint y, uint radius, uint row,
//        float start_slope, float end_slope, uint xx, uint xy, uint yx,
//        uint yy) {
//    if (start_slope < end_slope) {
//        return;
//    }
//    float next_start_slope = start_slope;
//    for (uint i = row; i <= radius; i++) {
//        bool blocked = false;
//        for (int dx = 0-i, dy = 0-i; dx <= 0; dx++) {
//            float l_slope = (dx - 0.5f) / (dy + 0.5f);
//            float r_slope = (dx + 0.5f) / (dy - 0.5f);
//            if (start_slope < r_slope) {
//                continue;
//            } else if (end_slope > l_slope) {
//                break;
//            }
//
//            int sax = dx * xx + dy * xy;
//            int say = dx * yx + dy * yy;
//            if ((sax < 0 && (uint)std::abs(sax) > x) ||
//                    (say < 0 && (uint)std::abs(say) > y)) {
//                continue;
//            }
//            uint ax = x + sax;
//            uint ay = y + say;
//            if (ax >= map.get_width() || ay >= map.get_height()) {
//                continue;
//            }
//
//            uint radius2 = radius * radius;
//            if ((uint)(dx * dx + dy * dy) < radius2) {
//                map.set_visible(ax, ay, true);
//            }
//
//            if (blocked) {
//                if (map.is_opaque(ax, ay)) {
//                    next_start_slope = r_slope;
//                    continue;
//                } else {
//                    blocked = false;
//                    start_slope = next_start_slope;
//                }
//            } else if (map.is_opaque(ax, ay)) {
//                blocked = true;
//                next_start_slope = r_slope;
//                cast_light(map, x, y, radius, i + 1, start_slope, l_slope, xx,
//                        xy, yx, yy);
//            }
//        }
//        if (blocked) {
//            break;
//        }
//    }
//}
