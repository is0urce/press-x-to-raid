#pragma once

#include "stdafx.h"

#include <memory>
#include <list>
#include <set>
#include <tuple>

#include "Scene.h"

namespace px
{

	struct Coord : public Point
	{
	public:
		//int X;
		//int Y;
		double g;
		double h;
		double f;
		Coord* prev;

	public:
		//Coord(int x, int y): X(x), Y(y), g(0), h(0), f(0), prev(0)
		//{
		//}
		//Coord(int x, int y, double heu): X(x), Y(y), g(0), h(heu), f(heu), prev(0)
		//{
		//}
		Coord(const Point &point): Point(point), g(0), h(0), f(0), prev(0)
		{
		}
		Coord(int x, int y): Point(x, y), g(0), h(0), f(0), prev(0)
		{
		}
		Coord(int x, int y, double heu): Point(x, y), g(0), h(heu), f(heu), prev(0)
		{
		}
		bool IsSamePosition(Coord c)
		{
			return X == c.X && Y == c.Y;
		}
		void Update(double cost, double heu, Coord* x)
		{
			g = cost;
			h = heu;
			f = g + h;
			prev = x;
		}
	};

	class CoordCompareScore
	{
	public:
		bool operator() (Coord* a, Coord* b)
		{
			return (a->f) < (b->f); // reverse 'less' to get lowest
		}
	};

	class CoordLess
	{
	public:
		bool operator() (Coord* a, Coord* b)
		{
			return std::tie(a->X, a->Y) < std::tie(b->X, b->Y);
		}
	};

	class SmartCoordCompareScore
	{
	public:
		bool operator() (std::shared_ptr<Coord> a, std::shared_ptr<Coord> b)
		{
			return (a->f) < (b->f); // reverse 'less' to get lowest
		}
	};

	class SmartCoordLess
	{
	public:
		bool operator() (std::shared_ptr<Coord> a, std::shared_ptr<Coord> b)
		{
			return std::tie(a->X, a->Y) < std::tie(b->X, b->Y);
		}
	};

	class Path
	{
	public:
		static std::list<Coord*>* Next(Coord c)
		{
			std::list<Coord*>* result = new std::list<Coord*>();
			result->push_back(new Coord(c.X + 1, c.Y));
			result->push_back(new Coord(c.X, c.Y + 1));
			result->push_back(new Coord(c.X - 1, c.Y));
			result->push_back(new Coord(c.X, c.Y - 1));
			// diagonals
			result->push_back(new Coord(c.X + 1, c.Y + 1));
			result->push_back(new Coord(c.X + 1, c.Y - 1));
			result->push_back(new Coord(c.X - 1, c.Y + 1));
			result->push_back(new Coord(c.X - 1, c.Y - 1));

			return result;
		}
		static std::list<Coord> ReConstructPath(Coord* step)
		{
			std::list<Coord> path;

			if (!step) throw std::runtime_error("A* ReConstructPath(from), from == x0");

			while (true)
			{
				if (!step->prev) return path; // not count current position as part of path
				path.push_front(Coord(step->X, step->Y));
				step = step->prev;
			}
		}
		static void Release(Coord* m)
		{
			delete m;
		}
		static std::list<Coord> Find(int sx, int sy, int dx, int dy, int steps, const vr::Scene &s)
		{
			return Find(Coord(sx, sy), Coord(dx, dy), steps, s);
		}
		static std::list<Coord> Find(Point start, Point finish, int steps, const vr::Scene &s)
		{
			return Find(Coord(start), Coord(finish), steps, s);
		}
		static std::list<Coord> Find(Coord start, Coord finish, int steps, const vr::Scene &s)
		{
			std::set<std::shared_ptr<Coord>, SmartCoordLess> closed;
			std::multiset<std::shared_ptr<Coord>, SmartCoordCompareScore> open;

			open.insert(std::shared_ptr<Coord>(new Coord(start.X, start.Y, H(start, finish))));

			while (steps > 0 && !open.empty())
			{
				auto xi = open.begin();
				auto x = *xi;

				// terminal
				if (x->IsSamePosition(finish))
				{
					auto path = ReConstructPath(x.get());
					return path;
				}

				open.erase(*xi);
				closed.insert(x);

				auto next = std::unique_ptr<std::list<Coord*>>(Next(*x));
				for (auto yi = next->begin(); yi != next->end(); ++yi)
				{
					auto y = std::shared_ptr<Coord>(*yi);
					int px = y->X;
					int py = y->Y;
					//if (s->IsOccupied(px, py) && (px != finish.X || py != finish.Y)) continue;
					if (!s.IsTraversable(Point(px, py)) && (px != finish.X || py != finish.Y)) continue;
					if (closed.find(y) != closed.end()) continue;

					double cost = x->g + Distance(*x, *y);
					y->Update(cost, H(*y, finish), x.get());

					bool find = false;
					for (auto it = open.begin(); it != open.end(); /* can't iterate if changed */ )
					{
						if ((*it)->IsSamePosition(*y))
						{
							find = true;
							if (cost < (*it)->g)
							{
								open.erase(it); // update /w key value
								open.insert(y);
							}
							it = open.end();
						}
						else
						{
							++it;
						}
					}
					if (!find)
					{
						open.insert(y);
					}
				}

				--steps;
			}
			// nothing found

			// construct path to best found vertice in border set if present (i.e. maybe there is a path & we're just reach step limit)
			auto xi = open.begin();
			if (xi != open.end())
			{
				//if (H(**xi, finish) < H(start, finish))
				{
					return ReConstructPath(xi->get());
				}
			}

			// we're in closed space, stay calm @ don't make mistake
			std::list<Coord> path;
			return path;
		}

		static double H(Coord a, Coord b)
		{
			//return std::abs(a.X - b.X) + std::abs(a.Y - b.Y);
			return std::sqrt(std::pow(a.X - b.X, 2.0) + std::pow(a.Y - b.Y, 2.0));
		}

		//...b...
		//../....
		//./.....
		///......
		//|......
		//a......
		//static double Distance(Coord a, Coord b)
		//{
		//	return std::max<int>(std::abs(a.X - b.X), std::abs(a.Y - b.Y));
		//	//return std::sqrt(std::pow(a.X - b.X, 2.0) + std::pow(a.Y - b.Y, 2.0));

		//	if (a.X != 0) return Distance(Coord(0, a.Y), Coord(b.X - a.X, b.Y));
		//	if (a.Y != 0) return Distance(Coord(a.X, 0), Coord(b.X, b.Y - a.Y));
		//	// a == (0,0) btw
		//	int x = std::abs(b.X);
		//	int y = std::abs(b.Y);
		//	// return x + y;
		//	return std::max<double>(x, y);
		//}
		static double Distance(Coord a, Coord b)
		{
			return 1.0;
		}
	};

}
// BONUS

//function A*(start,goal)
//     closedset := the empty set    // ��������� ������, ������� ��� ���� ����������(��������)
//     openset := {start}            // ��������� ������(�������), ������� ��������� ����������(��������). 
//	                               // ���������� ����� ������������ ������ ��������� ������� start.
//     path_map := the empty set     // ����� ���������� ������. ������������ �������� reconstruct_path
// 
//     //��������� �������� ������� start
//     start.g := 0   // g(x). ��������� ���� �� ��������� �������. � start g(x) = 0
//     start.h := heuristic_cost_estimate(start, goal) // ������������� ������ ���������� �� ����. h(x)
//     start.f := start.g + start.h      // f(x) = g(x) + h(x)
// 
//     while openset is not empty
//         x := ������� �� openset ������� ����� ������ ������ f(x)
//         if x = goal 
//             return reconstruct_path(start,goal) //��������� ����� path_map
// 
//         remove x from openset // ������� x ����� �� ���������, � ������ � ������� ������� �� ������� �� ���������
//         add x to closedset    // � �������� � ������ ��� ������������
//         foreach y in neighbor_nodes(x) // ��������� ������� ������ x
//             if y in closedset          // ���������� ������� �� ��������� ������
//                 continue
// 
//             tentative_g_score := x.g + dist_between(x,y)  // ��������� g(x) ��� ��������������� ������
// 
//             if y not in openset // ���� ����� x ��� �� � �������� ������ - ������� ��� ����
//                 add y to openset
//                 tentative_is_better := true
//             else               // ����� ��� � �������� ������, � ������ �� ��� ����� ��� g(x), h(x) � f(x)
//                 if tentative_g_score < y.g  
//                     // ����������� g(x) ��������� ������, � ������ ����� ����� ��������  g(x), h(x), f(x)
//                     tentative_is_better := true   
//                 else
//                     // ����������� g(x) ��������� ������, ��� ��������� � openset. 
//                     // ��� ��������, ��� �� ������� x ���� ����� ����� ������ ������
//                     // �.�. ���������� ����� ������� �������, ����������� ����� ����� ������ (�� �����-�� ������ �������, �� �� x)
//                     // ������� ������� ������ �� ����������
//                     tentative_is_better := false 
// 
//             // ���������� ������� ������. 
//             if tentative_is_better = true
//                 y.came_from := x //������� � ������� �� ������. ������������ ��� ������������� ����.
//                 y.g := tentative_g_score
//                 y.h := heuristic_cost_estimate(y, goal)
//                 y.f := y.g + y.h
//             // �������� ��������, ��� ���� ���������� ���������� ������� - ������ y(����� x) 
//             // ��� ��� ����� ��������� � openset.
//             // �.�. ��� ��������� �������� �������� ����� �� openset ����� ��������� ������� � ���������� ������� f(x)
//             // �� ���������, ��� ��� �������� ������� ������ x, �������� �� ������ ��� ��������.
//             // � ����� ��� ����� ������ ����������� ��������� �*
// 
//     return failure //���������� ��������� ���� ����� openset ����, � goal �� ������ (���� ����� �� �������)
// 
//// ��������� ����� path_map
//// ���� ����� ���������� ������ �� �������� �������(���� ����� ��� goal) 
//// � ������(������ ������� ����� �������� came_from, ��� �� � �������������)
//function reconstruct_path(start_node, goal_node)
//// ��������� � ����� ��� ������� �� finish_node �� start_node.
//     current_node := goal_node // ����� ���������� �� ������
//     while current_node <> NULL 
//             path_map.add_node(current_node) // �������� ������� � �����
//             current_node := current_node.came_from
// 
//     return path_map