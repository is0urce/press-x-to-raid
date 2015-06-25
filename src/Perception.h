// name: Perception.h
// type: c++ header
// desc: class declaration
// auth: is0urce

#include "Avatar.h"
#include "Map.h"
#include "Appearance.h"
#include "Color.h"
#include "Notification.h"

#pragma once

namespace px
{
	namespace vr
	{
		class Unit;
	}

	namespace shell
	{
		class Perception
		{
		public:
			// units
			typedef std::list<Avatar> UnitList;
			typedef UnitList::iterator UnitIterator;
			typedef std::pair<UnitIterator, UnitIterator> UnitRange;
			// notifications
			typedef std::wstring NotifyString;
			typedef vr::Notification<NotifyString> Notification;
			typedef std::list<Notification> NotifyList;
		private:
			Map<Appearance> _appearance;
			std::unique_ptr<Map<Color>> _color;
			std::unique_ptr<Map<Color>> _colorprev;
			UnitList _units;
			NotifyList _notifications;
			Point _movement;
			Point _start;

		public:
			Perception(const Point& range);
			~Perception();
		private:
			Perception(const Perception&); // disable copy

		public:
			int GetWidth() const;
			int GetHeight() const;
			bool InRange(Point position) const;

			const Appearance& GetAppearance(Point position) const;
			const Color& GetLight(Point position) const;
			const Color& GetPreviousLight(Point position) const;
			void SetAppearance(Point, const Appearance &tile);
			void SetLight(Point, const Color &light);

			void AddUnit(Avatar avatar);
			void AddUnit(Appearance appearance, Point position, Point previous);
			const UnitList& GetUnits() const;

			void AddNotification(Point position, Color color, NotifyString text);
			const NotifyList& GetNotifications() const;

			void SetStart(Point point);
			Point GetStart() const;
			Point GetMovement() const;

			std::unique_ptr<Perception> Reassemble(const Point& movement);
		};
	}
}