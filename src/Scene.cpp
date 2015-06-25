// name: Scene.cpp
// type: c++ source file
// desc: 'Scene' class implementation
// auth: is0urce

#include "stdafx.h"

#include "Scene.h"

#include "Actor.h"

using namespace px;
using namespace px::vr;

namespace
{
	const int cell_width = 100;
	const int cell_height = cell_width;
	const Point cell_range(cell_width, cell_height);
}

Scene::Scene() :
	_map(new Map<Tile<Appearance>>(cell_range))
{
}

Scene::~Scene()
{
}

Tile<Appearance>& Scene::GetTile(const Point& point)
{
	if (!_map->InRange(point))
	{
		return _default;
	}
	return _map->GetElement(point);
}

const Tile<Appearance>& Scene::ReadTile(const Point& point) const
{
	if (!_map->InRange(point))
	{
		return _default;
	}
	return _map->GetElement(point);
}

bool Scene::IsTransparent(const Point& point) const
{
	if (!ReadTile(point).IsTransparent()) return false;

	auto it = _objects.find(point);
	if (it == _objects.end()) return true;

	return it->second->IsTransparent();

	//return ReadTile(point).IsTransparent();

	//auto find = _objects.find(point);
	//// serch forward
	//for (auto it = find; it != _objects.end(); ++it)
	//{
	//	if (it->second->GetPosition() != point) break; // leaving key range
	//	if (!it->second->IsTransparent())
	//	{
	//		return false;
	//	}
	//}
	//// backward
	//for (auto it = find; it != _objects.begin(); /**/ )
	//{
	//	--it;

	//	if (it->second->GetPosition() != point) break; // leaving key range
	//	if (!it->second->IsTransparent())
	//	{
	//		return false;
	//	}
	//}
	return true;
}

bool Scene::IsTraversable(const Point& point) const
{
	if (!ReadTile(point).IsTraversable()) return false;

	auto it = _objects.find(point);
	if (it == _objects.end()) return true;

	//return false;
	return it->second->IsTraversable();
}

void Scene::Add(UnitPtr unit, const Point& position)
{
	unit->SetPosition(position);
	_objects.emplace(position, unit);
}

void Scene::Move(UnitPtr unit, const Point& position)
{
	Remove(unit);
	Add(unit, position);
}

void Scene::Remove(UnitPtr unit)
{
	auto hint = _objects.find(unit->GetPosition());
	if (hint == _objects.end()) throw std::runtime_error("assert 1: Scene::Remove unit not found or position invalid");
	if (hint->second != unit) throw std::runtime_error("assert 2: Scene::Remove unit position invalid or scene corrupted");
	_objects.erase(hint);
}

void Scene::Clear()
{
	_objects.clear();
}

int Scene::Count() const
{
	return _objects.size();
}

void Scene::Tick(int delta)
{
	//UnitList copy = _objects;
	//for (std::pair<Point, std::shared_ptr<Unit>> i : _objects)//= copy.begin(); i != copy.end();)
	//for (std::pair<Point, std::shared_ptr<Item>> i = copy.begin(); i != copy.end();)
	//for (auto i = _objects.begin(); i != _objects.end();)
	//{
	//	i->second->Tick(delta);
	//	if (i->second->IsDestroying())
	//	{
	//		i = _objects.erase(i);
	//	}
	//	else
	//	{
	//		++i;
	//	}
	//}

	//UnitList copy(_objects);
	for (const std::pair<Point, std::shared_ptr<Unit>> &i : UnitList(_objects))
	{
		i.second->Tick(delta);
	}
	for (auto i = _objects.begin(); i != _objects.end(); )
	{
		if (i->second->IsDestroying())
		{
			i = _objects.erase(i);
		}
		else
		{
			++i;
		}
	}
}

const Scene::UnitList& Scene::GetUnits() const
{
	return _objects;
}

Scene::UnitPtr Scene::GetBlockingUnit(const Point& place) const
{
	auto hint = _objects.find(place);
	return hint == _objects.end() ? nullptr : hint->second;
}