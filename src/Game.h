// name: Game.h
// type: c++ header
// desc: class declaration
// auth: is0urce

#pragma once

#include "IControl.h"
#include "Point.h"
#include "Broadcast.h"
#include "AbilityDirectory.h"

namespace px
{
	namespace vr
	{
		class Scene;
		class Unit;
		class Actor;
	}
	namespace shell
	{
		class Perception;
	}
	namespace ui
	{
		class Gui;
		class APanel;
	}
	class Shadow;

	namespace mech
	{
		class Game : public ui::IControl
		{
		public:
			typedef std::pair<Color, std::wstring> Message;
			typedef std::list<vr::Broadcast<Message>> BroadcastContainer;

		private:
			int _level;
			int _turn;
			bool _shutdown;
			std::unique_ptr<shell::Perception> _perception;
			std::unique_ptr<Shadow> _fov;
			std::unique_ptr<vr::Scene> _scene;
			std::shared_ptr<vr::Actor> _player;
			AbilityDirectory _skills;

			// ui
			std::unique_ptr<ui::Gui> _table;
			std::unique_ptr<ui::APanel> _panel;
			Point _hover;
			std::weak_ptr<vr::Unit> _target;
			BroadcastContainer _messages;

		public:
			Game();
			~Game();
		private:
			Game(const Game&); // disable copy

		protected:
			virtual bool KeyControl(int code);
			virtual bool HoverControl(int x, int y);
			virtual bool ClickControl(int x, int y, int button);
			virtual bool ScrollControl(int delta);

		private:
			void BeginPerception();
			void EndPerception();
			void Turn();
			void FillPreface();
			void Fill(int level);

		public:
			void Step(Point step);
			void Use(int skill);
			void Activate();
			void Start(int level);
			void End();
			bool IsFinished() const;
			int GetTurn() const;
			void NextLevel();

			ui::APanel* GetInterface() const;
			vr::Scene* GetScene() const;
			std::shared_ptr<vr::Actor> GetPlayer();
			std::shared_ptr<vr::Unit> GetTarget();
			const AbilityDirectory& GetSkills();
			Point GetHoverPoint() const;
			const shell::Perception& GetPerception() const;
			void Broadcast(Point position, Message message);
			void Broadcast(vr::Unit &unit, Message message);
		};
	}
}
