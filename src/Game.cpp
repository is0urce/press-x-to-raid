// name: Game.cpp
// type: c++ code
// desc: 'Game' class implementation
// auth: is0urce

#include "stdafx.h"

#include "Game.h"

#include "Scene.h"
#include "Actor.h"
#include "Perception.h"
#include "Gui.h"
#include "MainPanel.h"

#include "Weapon.h"
#include "Armor.h"
#include "Ability.h"
#include "Portal.h"
#include "Chest.h"

#include "String.h"
#include "Automation.h"
#include "Shadow.h"
#include "AStar.h"

#include "EquipmentSlot.h"
#include "Attribute.h"

using namespace px;
using namespace px::mech;

// CONSTANTS & HELPERS

namespace
{
	const int perception_width = 50;
	const int perception_height = perception_width;
	const int shadowcast_range = perception_width;
	const Point perception_range(perception_width, perception_height);

	inline Attribute generate_mainstat()
	{
		switch (std::rand() % 3)
		{
		case 0:
			return Attribute::strength;
		case 1:
			return Attribute::intelligence;
		case 2:
			return Attribute::dexterity;
		}
		throw std::runtime_error("generate_mainstat()");
	}
	inline Attribute generate_thoughtness()
	{
		switch (std::rand() % 3)
		{
		case 0:
			return Attribute::vitality;
		case 1:
			return Attribute::dodge_rating;
		case 2:
			return Attribute::resist_rating;
		}
		throw std::runtime_error("generate_thoughtness()");
	}
	inline Attribute generate_dps()
	{
		switch (std::rand() % 3)
		{
		case 0:
			return Attribute::critical_rating;
		case 1:
			return Attribute::mastery_rating;
		case 2:
			return Attribute::accuracy_rating;
		}
		throw std::runtime_error("generate_dps()");
	}
	inline std::shared_ptr<vr::Weapon> generate_weapon(int level)
	{
		auto item = std::make_shared<vr::Weapon>();

		item->Enchant(Enhancement(Attribute::damage, 5 + level));
		item->Enchant(Enhancement(generate_thoughtness(), 5 + level));
		item->Enchant(Enhancement(generate_dps(), 5 + level));

		std::stringstream ss;
		ss << "T" << level << " Weapon";
		item->SetName(ss.str());
		item->SetAppearance('w');

		return item;
	}
	inline std::shared_ptr<vr::Armor> generate_armor(int level)
	{
		auto item = std::make_shared<vr::Armor>();

		item->Enchant(Enhancement(Attribute::armor, 5 + level));
		item->Enchant(Enhancement(generate_mainstat(), 5 + level));
		item->Enchant(Enhancement(generate_thoughtness(), 5 + level));
		item->Enchant(Enhancement(generate_dps(), 5 + level));

		std::stringstream ss;
		ss << "T" << level << " Armor";
		item->SetName(ss.str());
		item->SetAppearance('a');

		return item;
	}
	inline std::shared_ptr<vr::Actor> generate_monster(int level, const AbilityDirectory &skills)
	{
		auto mob = std::make_shared<vr::Actor>();

		int specie = std::rand() % 18;
		Appearance::Image image;
		std::unique_ptr<Ability> secondary;
		std::string name;
		
		switch (specie)
		{
		case 0:
			image = 'b';
			name = "Bear";
			break;
		case 1:
			image = 'f';
			name = "Fox";
			break;
		case 2:
			image = 'l';
			name = "Leopard";
			break;
		case 3:
			image = 'r';
			name = "Rat";
			break;
		case 4:
			image = 's';
			name = "Spider";
			break;
		case 5:
			image = 'v';
			name = "Vampire Bat";
			break;
		case 6:
			image = 'g';
			name = "Goblin";
			break;
		case 7:
			image = 'k';
			name = "Kobold";
			break;
		case 8:
			image = 'o';
			name = "Orc";
			break;
		case 9:
			image = 't';
			name = "Troll";
			break;
		case 10:
			image = 'n';
			name = "Naga";
			break;
		case 11:
			image = 'p';
			name = "Pixie";
			break;
		case 12:
			image = 'a';
			name = "Assassin";
			break;
		case 13:
			image = 'c';
			name = "Cultist";
			break;
		case 14:
			image = 'm';
			name = "Marauder";
			break;
		case 15:
			image = 'i';
			name = "Imp";
			break;
		case 16:
			image = 'z';
			name = "Zombie";
			break;
		case 17:
			image = 'd';
			name = "Demon";
			break;

		default:
			image = 'r';
			name = "Cellar Rat";
			break;
		}

		mob->SetFaction(1); // hostile

		// appearance
		mob->SetAppearance(Appearance(image, Color(1, 0, 0)));
		mob->SetName(name);

		// stats
		mob->ModAttribute(mech::Attribute::damage, level - 1 + 2);
		mob->ModAttribute(mech::Attribute::vitality, level / 2);
		mob->SetHealth(mob->GetMaximumHealth());
		mob->SetEnergy(100);

		// skills
		mob->SwapSkill(0, std::unique_ptr<Ability>(new Ability(skills.GetDescriptor("Meelee"), mob)));
		mob->SwapSkill(1, std::move(secondary));

		return mob;
	}

	// 0 - empty, positive - filled with distance, negative - walls
	inline void FloodFill(const Map<bool> &walls, const Point &start, int max, int &range, Point& last)
	{
		Map<int> flood(walls.GetRange());
		flood.Fill<bool>(walls, [] (bool c) { return ((c) ? -1 : 0); });
		flood.GetElement(start) = 1;

		last = start;
		bool changed = true;
		int wave = 1;
		while (changed && max > 0)
		{
			changed = false;
			flood.FillIndexed([&] (const Point& p)
			{
				int element = flood.GetElement(p);
				if (element == 0)
				{
					int outer = -1;
					if (flood.GetElement(p.Moved(Point( 0, -1)), outer) == wave ||
						flood.GetElement(p.Moved(Point( 0,  1)), outer) == wave ||
						flood.GetElement(p.Moved(Point(-1,  0)), outer) == wave ||
						flood.GetElement(p.Moved(Point( 1,  0)), outer) == wave ||
						flood.GetElement(p.Moved(Point( 1,  1)), outer) == wave ||
						flood.GetElement(p.Moved(Point(-1, -1)), outer) == wave ||
						flood.GetElement(p.Moved(Point( 1, -1)), outer) == wave ||
						flood.GetElement(p.Moved(Point(-1,  1)), outer) == wave)
					{
						changed = true;
						if (wave > range)
						{
							range = wave;
							last = p;
						}
						return wave + 1;
					}
				}
				return element;
			});
			++wave;
			--max;
		}
	}

	inline void FillShelf(const Point &point, vr::Scene &scene, std::string text)
	{
		int i = 0;
		String::enum_utf8(text, [&] (unsigned int letter)
		{
			auto &tile = scene.GetTile(point.Moved(Point(i % 7, 6 - i / 7 * 2)));
			if (letter != ' ')
			{
				tile.GetAppearance().SetImage(letter);
				tile.MakeWall();
			}
			else
			{
				tile.GetAppearance().SetImage('=');
				tile.SetTransparent(true);
				tile.SetTraversable(false);
			}
			++i;
		});
	}

}

// ctor & dtor

Game::Game() : 
	_scene(new vr::Scene()),
	_fov(new Shadow(shadowcast_range)),
	_perception(new shell::Perception(perception_range)),
	_table(new ui::Gui()),
	_shutdown(false),
	_turn(-1)
{
	_skills.Load(*this);
	_panel.reset(new ui::MainPanel(_table.get(), this));
}

Game::~Game()
{
}

// methods

void Game::End()
{
	_shutdown = true;
}

bool Game::IsFinished() const
{
	return _shutdown;
}

void Game::Start(int level)
{
	// insert player
	_player.reset(new vr::Actor());
	_player->SetAppearance(Appearance(L'@'));
	_player->SetHealth(100);
	_player->SetEnergy(100);
	_player->SetPerceptionDelegate([this] ()
	{
		_player->RestoreEnergy(5);
	});

	// equipment
	auto weapon = std::make_shared<vr::Weapon>();
	weapon->SetName("T0 Sword");
	weapon->Enchant(Enhancement(mech::Attribute::damage, 5));
	auto armor = std::make_shared<vr::Armor>();
	armor->SetName("T0 Armor");
	armor->Enchant(Enhancement(mech::Attribute::armor, 5));
	_player->Collect(weapon);
	_player->Collect(armor);
	_player->Equip(weapon);
	_player->Equip(armor);

	// skills
	_player->SwapSkill(0, std::unique_ptr<Ability>(new Ability(_skills.GetDescriptor("Meelee"), _player)));
	_player->SwapSkill(1, std::unique_ptr<Ability>(new Ability(_skills.GetDescriptor("Power Strike"), _player)));
	_player->SwapSkill(2, std::unique_ptr<Ability>(new Ability(_skills.GetDescriptor("Zap"), _player)));
	_player->SwapSkill(3, std::unique_ptr<Ability>(new Ability(_skills.GetDescriptor("Aim"), _player)));
	_player->SwapSkill(4, std::unique_ptr<Ability>(new Ability(_skills.GetDescriptor("Panacea"), _player)));

	// create level
	_scene->Clear();
	_level = level;
	if (level == 0)
	{
		FillPreface();
	}
	else
	{
		Fill(level);
	}

	_turn = 0;
	_player->StorePosition();
	_perception.reset(new shell::Perception(perception_range));
	_perception->SetStart(_player->GetPosition() - Point(perception_width / 2, perception_height / 2));
	EndPerception();
}

void Game::FillPreface()
{
	// fill scene
	const int w = 80;
	const int h = 80;
	Point(w, h).EnumerateRange([this, w, h] (Point p)
	{
		auto &t = _scene->GetTile(p);
		if (p.X == 7 || p.Y == 7 || p.X == w - 1 || p.Y == h - 1)
		{
			t.GetAppearance().SetImage('#');
			t.MakeWall();
		}
		else
		{
			t.GetAppearance().SetImage('.');
			t.MakeGround();
		}

		FillShelf(Point(10, 70), *_scene, "НАЧАЛООТДЕЛАБИБЛИОТЕКИ НАХОД");
		FillShelf(Point(10, 60), *_scene, "ИТСЯНА СЕВЕРО ВОСТОКЕ НОВРЯД"); // boxing wizard
		FillShelf(Point(10, 50), *_scene, "ЛИ ЕГО МОЖНО СЧИТАТЬ ОТПРАВН");
		FillShelf(Point(10, 40), *_scene, "ОЙ ТОЧКОЙ НА ПЕРВЫЙ ВЗГЛЯДКН");
		FillShelf(Point(10, 30), *_scene, "ИГИ РАСПОЛОЖЕНЫ БЕЗКАКОЙЛИБО");
		FillShelf(Point(10, 20), *_scene, " СИСТЕМЫ НО ПОНЯТНО ЧТО НА");
		FillShelf(Point(10, 10), *_scene, "ЛЕВО ИСТОРИЯ НАПРАВООБУЧЕНИЕ");

		FillShelf(Point(20, 10), *_scene, "БОКСИРУЮИЩЙВОЛШЕБНИКЭТОНОРМА");
		FillShelf(Point(20, 20), *_scene, "LOREMIPSUMDOLORSITAMETCONSEC");
		FillShelf(Point(20, 30), *_scene, "LOREMIPSUMDOLORSITAMETCONSEC");
		FillShelf(Point(20, 40), *_scene, "LOREMIPSUMDOLORSITAMETCONSEC");
		FillShelf(Point(20, 50), *_scene, "LOREMIPSUMDOLORSITAMETCONSEC");
		FillShelf(Point(20, 60), *_scene, "LOREMIPSUMDOLORSITAMETCONSEC");
		FillShelf(Point(20, 70), *_scene, "LOREMIPSUMDOLORSITAMETCONSEC");

		FillShelf(Point(30, 70), *_scene, "LOREMIPSUMDOLORSITAMETCONSEC");
		FillShelf(Point(30, 60), *_scene, "ОТ КРАЯДО КРАЯ СТОЯТ ШКАФЫ С"); // SHELFS
		FillShelf(Point(30, 50), *_scene, "Й  ВИД ЭТИКНИГИОЧЕНЬСТАРЫЕБУ"); // view this books
		FillShelf(Point(30, 40), *_scene, "ДТОХРАНЯТСЯЗДЕСЬ С НАЧАЛАВРЕ"); // beginning of time
		FillShelf(Point(30, 30), *_scene, "МЁН НЕКОТО ЫЕ ПОЛКИ ПУСТ УЮТ"); // some shelfs are empty
		FillShelf(Point(30, 20), *_scene, "БУД ТО КТО ТО У ТА ЩИ Л В С "); // as if someone bring
		FillShelf(Point(30, 10), *_scene, "Ё В  Н         И          З "); // all down

		FillShelf(Point(40, 70), *_scene, "LOREMIPSUMDOLORSITAMETCONSEC");
		FillShelf(Point(40, 60), *_scene, "ХРАНИЛИЩЕ ВСЕХВСЕХ ВСЕХ КНИГ"); // vault of everrrrryone
		FillShelf(Point(40, 50), *_scene, "СВЕРХУ ДЖУНГЛИ ВНИЗУ ТОННЕЛИ"); // up - jungles, down - tonnails
		FillShelf(Point(40, 40), *_scene, "ВПЕРЕДИБЕСКОНЕЧНЫЙ СПУСКВНИЗ"); // endless
		FillShelf(Point(40, 30), *_scene, "ЧЕМ ГЛУБЖЕ ТЕМ БУДЕТ ОПАСНЕЕ"); // deeper - danger
		FillShelf(Point(40, 20), *_scene, "ТОМА С УМЕНИЯМИ ПОМОГУТ ВСЁ "); // skillbooks
		FillShelf(Point(40, 10), *_scene, "П   Р Е В   О З   М   О Ч  Ь"); // HELPS

		FillShelf(Point(50, 70), *_scene, "LOREMIPSUMDOLORSITAMETCONSEC");
		FillShelf(Point(50, 60), *_scene, "ОДНОВРЕМЕННО МОЖНО ИСПОЛЬЗОВ");
		FillShelf(Point(50, 50), *_scene, "АТЬ ДО ЧЕТЫРЁХ РАЗНЫХ УМЕНИЙ");
		FillShelf(Point(50, 40), *_scene, "КОМБИНИРУЙТЕ ПОДХОДЯЩИЕ УМЕН");
		FillShelf(Point(50, 30), *_scene, "ИЯ ДЛЯ НАЙДЕННОЙ ЭКИПИРОВКИ ");
		FillShelf(Point(50, 20), *_scene, "РАЗНЫЕ ХАРАКТЕРИСТИКИ ДЕЛАЮТ");
		FillShelf(Point(50, 10), *_scene, "РАЗЛИЧНЫЕ    УМЕНИЯ  СИЛЬНЕЕ");

		FillShelf(Point(60, 10), *_scene, "LOREMIPSUMDOLORSITAMETCONSEC");
		FillShelf(Point(60, 20), *_scene, "LOREM IPSUM DOLOR SIT AMET  ");
		FillShelf(Point(60, 30), *_scene, "LOREM IPSUM DOLOR SIT AMET  ");
		FillShelf(Point(60, 40), *_scene, "LOREMIPSUMDOLORSITAMETCONSEC");
		FillShelf(Point(60, 50), *_scene, "LOREM IPSUM DOLOR SIT AMET  ");
		FillShelf(Point(60, 60), *_scene, "LOREM IPSUM DOLOR SIT AMET  ");
		FillShelf(Point(60, 70), *_scene, "LOREM IPSUM DOLOR SIT AMET  ");

		FillShelf(Point(70, 70), *_scene, "LOREMIPSUMDOLORSITAMETCONSEC");
		FillShelf(Point(70, 60), *_scene, " БРОСЬ ПУСТЫЕ ВИДЕТЬ  С Н Ы"); // stop dreaming
		FillShelf(Point(70, 50), *_scene, "LOREM IPSUM DOLOR SIT AMET  ");
		FillShelf(Point(70, 40), *_scene, "ВСЁ ЧТОИМЕЕТНАЧАЛОИМЕЕТКОНЕЦ"); // begin an end
		FillShelf(Point(70, 30), *_scene, " ЖИЗНЬ  Э О     П ТЕ ШЕСТВИЕ"); // life is a trip
		FillShelf(Point(70, 20), *_scene, "Н СТОЯЩЕЕ  Э Т  О    ПОДАРОК"); // present is present
		FillShelf(Point(70, 10), *_scene, "В К НЦЕ Т  М Р  Ё  Ш    Ь   ");
	});

	// generate props & mobs
	Point exit(25, 8);
	auto hole = std::make_shared<vr::Portal>(this);
	hole->SetPosition(exit, exit);
	hole->SetAppearance(Appearance('O', Color(1, 1, 1)));
	_scene->Add(hole, exit);


	// insert player
	_scene->Add(_player, Point(38, 58));
}

void Game::Fill(int level)
{
	// generate
	int w = 35;
	int h = 35;
	Point start;
	Point exit;
	Automation<bool> walls(Point(w, h));
	bool generated = false;
	while (!generated)
	{
		walls.FillIndexed([w,h] (const Point& p) { return p.X == 0 || p.Y == 0 || p.X == w - 1 || p.Y == h - 1 || std::rand() % 100 < 42; });
		walls.Execute<int>([] (int summ, bool element) { return summ + (element ? 1 : 0); }, 0, [] (int summ) { return (summ == 0 || summ >= 5) ? true : false; }, 4);

		// select start point
		for (int i = 0; i < w; ++i)
		{
			if (!walls.GetElement(Point(i, h / 3)))
			{
				start = Point(i, h / 3);
				break;
			}
			if (!walls.GetElement(Point(i, h * 2 / 3)))
			{
				start = Point(i, h * 2 / 3);
				break;
			}
		}

		// check
		int distance = 0;
		FloodFill(walls, start, w * h, distance, exit);

		if (distance > (w + h) / 4)
		{
			generated = true;
		}
	}

	Automation<bool> wall_overlay(Point(w, h));
	wall_overlay.FillIndexed([w,h] (const Point& p) { return p.X == 0 || p.Y == 0 || p.X == w - 1 || p.Y == h - 1 || std::rand() % 100 < 42; });
	wall_overlay.Execute<int>([] (int summ, bool element) { return summ + (element ? 1 : 0); }, 0, [] (int summ) { return (summ == 0 || summ >= 5) ? true : false; }, 4);
	int overlay = std::rand() % 3;

	// fill scene
	Point::EnumerateRectangle(walls.GetRange(), [&] (const Point &p)
	{
		if (walls.GetElement(p))
		{
			if (!wall_overlay.GetElement(p))
			{
				switch (overlay)
				{
				case 0:
					_scene->GetTile(p).GetAppearance().SetImage('~');
					_scene->GetTile(p).GetAppearance().SetColor(0x0000ff);
					_scene->GetTile(p).SetTransparent(true);
					_scene->GetTile(p).SetTraversable(false);
					break;
				case 1:
					_scene->GetTile(p).GetAppearance().SetImage('#');
					_scene->GetTile(p).GetAppearance().SetColor(0x3333ff);
					_scene->GetTile(p).MakeWall();
					break;
				default :
					_scene->GetTile(p).GetAppearance().SetImage('#');
					_scene->GetTile(p).GetAppearance().SetColor(Color(1, 1, 1));
					_scene->GetTile(p).MakeWall();
				}
			}
			else
			{
				_scene->GetTile(p).GetAppearance().SetImage('#');
				_scene->GetTile(p).GetAppearance().SetColor(Color(1, 1, 1));
				_scene->GetTile(p).MakeWall();
			}
		}
		else
		{
			if (overlay == 1 && wall_overlay.GetElement(p))
			{
				_scene->GetTile(p).GetAppearance().SetImage('.');
				_scene->GetTile(p).GetAppearance().SetColor(0x3333ff);
				_scene->GetTile(p).MakeGround();
			}
			else
			{
				_scene->GetTile(p).GetAppearance().SetImage('.');
				_scene->GetTile(p).GetAppearance().SetColor(Color(1, 1, 1));
				_scene->GetTile(p).MakeGround();
			}
		}
	});

	// insert player
	_scene->Add(_player, start);

	// generate props & mobs
	auto hole = std::make_shared<vr::Portal>(this);
	hole->SetPosition(exit, exit);
	hole->SetAppearance(Appearance('O', Color(1, 1, 1)));
	_scene->Add(hole, exit);

	int enemies = w * h / 49;
	for (int i = 0; i < enemies; ++i)
	{
		Point position;
		for (bool select = false; !select; select = _scene->IsTraversable(position))
		{
			position = Point(std::rand() % w, std::rand() % h);
		}
		auto mob = generate_monster(level, _skills);//std::make_shared<vr::Actor>();
		mob->SetPosition(position, position);

		// combat delegate
		int state = 0;
		int detect_distance = 8;
		mob->SetPerceptionDelegate([mob,state,detect_distance,this] () mutable
		{
			Point start = mob->GetPosition();
			Point dest = _player->GetPosition();
			bool handle = start.KingDistanceTo(dest) < detect_distance || mob->GetCurrentHealth() != mob->GetMaximumHealth();//false;//abs(x - _player->X) + abs(y - _player->Y) > 25;
			if (handle && !mob->IsDead() && mob->GetReputation(*_player) < 0)
			{
				if (state == 0 && _fov->IsInFOV(mob->GetPosition()))
				{
					state = 1;
					Broadcast(*mob, Message(Color(1, 0, 0), L"!"));
				}

				if (state != 0)
				{
					Point start = mob->GetPosition();
					Point dest = _player->GetPosition();

					Point move(1, 0);

					auto path = Path::Find(start, dest, 50, *_scene);
					auto step = path.begin();
					if (step != path.end()) // get first step if exists
					{
						move = *step;
					}

					// don't stomp on your friends
					auto versus = _scene->GetBlockingUnit(move);
					if (versus && mob->GetReputation(*versus) < 0)
					{
						//Action(unit, mx, my);
						auto skill = mob->GetSkill(1);
						if (skill && skill->CanUse(versus))
						{
							skill->Use(versus);
						}
						else
						{
							auto primary = mob->GetSkill(0);
							if (primary && primary->CanUse(versus))
							{
								primary->Use(versus);
							}
						}
					}
					else
					{
						_scene->Move(mob, move);
					}
				}
			}
		});

		_scene->Add(mob, position);
	}

	int loot = 4 + std::rand() % 2;
	for (int i = 0; i < loot; ++i)
	{
		Point position;
		for (bool select = false; !select; select = _scene->IsTraversable(position))
		{
			position = Point(std::rand() % w, std::rand() % h);
		}
		auto chest = std::make_shared<vr::Chest>(this, std::rand() % 2 == 0 ? (std::shared_ptr<vr::Item>)generate_armor(level) : (std::shared_ptr<vr::Item>)generate_weapon(level));
		chest->SetPosition(position, position);
		chest->SetAppearance(Appearance('c', Color(1, 1, 0)));

		_scene->Add(chest, position);
	}
}

void Game::NextLevel()
{
	_scene->Clear();
	Fill(++_level);

	_turn = 0;
	_player->StorePosition();
	_perception.reset(new shell::Perception(perception_range));
	_perception->SetStart(_player->GetPosition() - Point(perception_width / 2, perception_height / 2));
	EndPerception();
}

void Game::EndPerception()
{
	if (_player == nullptr) throw std::runtime_error("Game::FillPerception() - game not initialized");

	Point start = _player->GetPosition() - Point(perception_width / 2, perception_height / 2);
	Color ambient = Color::White();

	_fov->Cast(*_scene, _player->GetPosition());

	_perception = _perception->Reassemble(start - _perception->GetStart());

	Point::EnumerateRectangle(perception_range, [&] (Point position)
	{
		Point absolute = position + start;
		_perception->SetAppearance(position, _scene->GetTile(absolute).GetAppearance());
		if (_fov->IsInFOV(absolute))
		{
			_perception->SetLight(position, ambient);
		}
		else
		{
			_perception->SetLight(position, 0);
		}
	});

	for (const auto &u : _scene->GetUnits())
	{
		if (_fov->IsInFOV(u.first))
		{
			Point position = u.first - start;
			if (_perception->InRange(position))
			{
				Appearance occupied = _scene->GetTile(u.first).GetAppearance();
				occupied.SetImage(' ');
				_perception->SetAppearance(position, occupied);
				//_perception->SetLight(position, 0);
			}
			_perception->AddUnit(u.second->GetAppearance(), position, u.second->GetPreviousPosition() - start);
		}
	}

	for (auto i : _messages)
	{
		auto message = i.GetMessage();
		_perception->AddNotification(i.GetPosition() - start, message.first, message.second);
	}
	_messages.clear();
}

void Game::Broadcast(Point position, Message message)
{
	_messages.emplace_back(position, message);
}

void Game::Broadcast(vr::Unit &unit, Message message)
{
	Broadcast(unit.GetPosition(), message);
	//Broadcast(unit.GetPreviousPosition(), message);
}

const shell::Perception& Game::GetPerception() const
{
	if (_perception == nullptr) throw std::runtime_error("Game::GetPerception() perception not initialized");

	return *_perception.get();
}

ui::APanel* Game::GetInterface() const
{
	return _panel.get();
}

void Game::BeginPerception()
{
	for (const auto &u : _scene->GetUnits())
	{
		u.second->StorePosition();
	}
}

void Game::Step(Point point)
{
	if (_player == nullptr || _player->IsDead()) return;

	Point position = _player->GetPosition() + point;

	if (_scene->IsTraversable(position))
	{
		BeginPerception();
		_scene->Move(_player, position);
		Turn();
		EndPerception();
	}
	else
	{
		auto blocking = _scene->GetBlockingUnit(position);
		if (blocking)
		{
			if (blocking == _player)
			{
				BeginPerception();
				Turn();
				EndPerception();
			}
			else
			{
				_target = blocking;
				Use(0);
			}
		}
		else // wall
		{
			BeginPerception();
			EndPerception();
		}
	}
}

void Game::Use(int n)
{
	if (_player == nullptr || _player->IsDead()) return;

	BeginPerception();
	auto skill = _player->GetSkill(n);
	if (skill)
	{
		auto target = _target.lock();
		if (skill->IsTargeted() && target)
		{
			if (skill->CanUse(target))
			{
				skill->Use(target);
				Turn();
			}

		}
		else
		{
			if (skill->CanUse(_hover))
			{
				skill->Use(_hover);
				Turn();
			}
		}
	}
	EndPerception();
}

void Game::Activate()
{
	auto target = _target.lock();
	if (target && _player->GetPosition().KingDistanceTo(target->GetPosition()) == 1)
	{
		target->Use(*_player);
	}
}

bool Game::KeyControl(int code)
{
	bool result = true;

	switch (code)
	{
	case '1':
		Use(1);
		break;
	case '2':
		Use(2);
		break;
	case '3':
		Use(3);
		break;
	case '4':
		Use(4);
		break;
	case VK_NUMPAD8:
	case VK_UP:
	case 'W':
		Step(Point(0, 1));
		break;
	case VK_NUMPAD4:
	case VK_LEFT:
	case 'A':
		Step(Point(-1, 0));
		break;
	case VK_NUMPAD2:
	case VK_DOWN:
	case 'S':
		Step(Point(0, -1));
		break;
	case VK_NUMPAD6:
	case VK_RIGHT:
	case 'D':
		Step(Point(1, 0));
		break;
	case VK_NUMPAD7:
	case VK_HOME:
		Step(Point(-1, 1));
		break;
	case VK_NUMPAD9:
	case VK_PRIOR:
		Step(Point(1, 1));
		break;
	case VK_NUMPAD1:
	case VK_END:
		Step(Point(-1, -1));
		break;
	case VK_NUMPAD3:
	case VK_NEXT:
		Step(Point(1, -1));
		break;
	case VK_SPACE:
	case VK_NUMPAD5:
		Step(Point(0, 0));
		break;
	case 'E':
		Activate();
		break;
	case 'R':
		for (int i = 0; i < 10 && _player->GetCurrentEnergy() < _player->GetMaximumEnergy(); ++i)
		{
			Step(Point(0, 0));
		}
		break;
	case VK_F12:
		Start(1);
		break;
	case VK_ESCAPE:
		End();
		break;
	default:
		result = false;
	}

	return result;
}

bool Game::HoverControl(int x, int y)
{
	if (_player)
	{
		_hover = Point(x, y) + _player->GetPosition();
		_target = _scene->GetBlockingUnit(_hover);
		return true;
	}
	return false;
}

bool Game::ClickControl(int x, int y, int button)
{
	if (_player && button == 1)
	{
		HoverControl(x, y);
		auto skill = _player->GetSkill(0);
		auto target = _target.lock();
		if (target && skill && skill->CanUse(target))
		{
			Use(0);
		}
		else
		{
			int dx = (x == 0 || std::abs(x) < std::abs(y) / 2) ? 0 : x > 0 ? 1 : -1;
			int dy = (y == 0 || std::abs(y) < std::abs(x) / 2) ? 0 : y > 0 ? 1 : -1;
			Step(Point(dx, dy)); 
		}
		return true;
	}

	return false;
}

bool Game::ScrollControl(int delta)
{
	return false;
}

void Game::Turn()
{
	_scene->Tick(1);
	++_turn;
}

vr::Scene* Game::GetScene() const
{
	return _scene.get();
}

std::shared_ptr<vr::Actor> Game::GetPlayer()
{
	return _player;
}

std::shared_ptr<vr::Unit> Game::GetTarget()
{
	auto target = _target.lock();
	if (!target || !_fov->IsInFOV(target->GetPosition())) return nullptr;
	return target;
}

const AbilityDirectory& Game::GetSkills()
{
	return _skills;
}

Point Game::GetHoverPoint() const
{
	return _hover;
}

int Game::GetTurn() const
{
	return _turn;
}