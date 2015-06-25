// name: AbilityDirectory.cpp
// type: c++
// desc: class implementation
// auth: is0urce

// container for available ability templates

#include "stdafx.h"

#include "AbilityDirectory.h"

#include "Game.h"
#include "Actor.h"
#include "Unit.h"

#include "Scene.h"
#include "StatusEffect.h"

#include "Attribute.h"

using namespace px;
using namespace px::mech;

// helper functions

namespace
{
	// calculate diminishing - 5% for every 5 rating
	// formulae(x) = 1/(0.0105263157894737*x+1)
	// result in [0, 1]
	// f(0) = 1
	// f(+inf) = 0
	inline double diminishing(double rating)
	{
		return 1.0 / (0.0105263157894737 * rating + 1.0);
	}
	inline double diminish(double value, double rating)
	{
		return value *= (1.0 / (0.0105263157894737 * rating + 1.0));
	}

	inline bool attack_roll(double dodge_rating)
	{
		const unsigned int quant = 1 << 15;
		const double dodge_base = 0.05; // roll 20!
		return std::rand() % quant < (diminishing(dodge_rating) - dodge_base) * quant;
	}

	inline bool attack_roll(const vr::Actor &attacker, const vr::Actor &dodger)
	{
		int dodge = attacker.GetAttribute(Attribute::dodge_rating) - dodger.GetAttribute(Attribute::accuracy_rating);
		return attack_roll(dodge);
	}

	inline bool critical_roll(const vr::Actor &attacker)
	{
		int crit_rating = attacker.GetAttribute(Attribute::critical_rating);
		const unsigned int quant = 1 << 15;
		const double crit_base = 0.05; // roll 20!
		return std::rand() % quant > (diminishing(crit_rating) - crit_base) * quant;
	}

	inline double calculate_damage(const vr::Actor &attacker, const vr::Actor &target, Attribute specialization)
	{
		int weapon_damage = attacker.GetAttribute(mech::Attribute::damage) + attacker.GetAttribute(specialization);
		int resist = target.GetAttribute(Attribute::resist_rating);
		double damage = diminish(weapon_damage, resist);

		// crit
		if (critical_roll(attacker)) damage *= 2;

		// mastery
		int mastery_rating = attacker.GetAttribute(Attribute::mastery_rating);
		damage *= (2.0 - diminishing(mastery_rating));

		return damage;
	}
}

// ctor & dtor

AbilityDirectory::AbilityDirectory()
{
}

AbilityDirectory::~AbilityDirectory()
{
}

// methods

void AbilityDirectory::Load(Game &game)
{
	Descriptor::TargetFunctionCheck meelee_check([&] (User user, Target target)
	{
		return user->GetPosition().KingDistanceTo(target->GetPosition()) == 1;
	});
	Descriptor::TargetFunctionCheck hostile_check([&] (User user, Target target)
	{
		return user != target;
	});
	Descriptor::GroundFunctionCheck ground_true([] (User user, const Point& point) -> bool
	{
		return true;
	});

	// meelee attack
	Descriptor::TargetFunction meelee_action([&game] (User user, Target target)
	{
		Status<vr::Actor> damage(0, "Meelee wound", nullptr, [&game,user] (vr::Actor &actor, int duration)
		{
			if (attack_roll(*user, actor))
			{
				double weapon_damage = calculate_damage(*user, actor, Attribute::strength);
				int damage = int(std::ceil(weapon_damage));
				actor.Damage(damage);

				// broadcast
				std::wostringstream ss;
				ss << L"-" << damage;
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), ss.str()));
			}
			else
			{
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), L"* miss *"));
			}
		}, nullptr);

		target->ApplyEffect(StatusEffect(damage));
	});

	// Power Strike – 200% weapon damage – 20 mana
	Descriptor::TargetFunction power_strike_action([&game] (User user, Target target)
	{
		Status<vr::Actor> damage(1, nullptr, [&game,user] (vr::Actor &actor, int duration)
		{
			if (attack_roll(*user, actor))
			{
				double weapon_damage = calculate_damage(*user, actor, Attribute::strength);
				weapon_damage *= 2;
				int damage = int(std::ceil(weapon_damage));
				actor.Damage(damage);

				// broadcast
				std::wostringstream ss;
				ss << L"-" << damage;
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), ss.str()));
			}
			else
			{
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), L"* miss *"));
			}
		}, nullptr);
		target->ApplyEffect(StatusEffect(damage));
	});

	// Rend – 100% weapon damage + 25% weapon damage over 5 turns
	Descriptor::TargetFunction rend_action([&game] (User user, Target target)
	{
		Status<vr::Actor> damage(1, nullptr, [&game,user] (vr::Actor &actor, int duration)
		{
			if (attack_roll(*user, actor))
			{
				double weapon_damage = calculate_damage(*user, actor, Attribute::strength);
				int damage = int(std::ceil(weapon_damage));
				actor.Damage(damage);

				// apply dot
				Status<vr::Actor> dot(5, "Rend", [&game,user] (vr::Actor &actor, int duration)
				{
					auto weapon_damage = calculate_damage(*user, actor, Attribute::strength);
					int damage = int(std::ceil(weapon_damage * 0.25));
					actor.Damage(damage);

					// broadcast
					std::wostringstream ss;
					ss << L"-" << damage;
					game.Broadcast(actor, Game::Message(Color(1, 1, 0), ss.str()));
				});
				actor.ApplyEffect(StatusEffect(dot));

				// broadcast
				std::wostringstream ss;
				ss << L"-" << damage;
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), ss.str()));
			}
			else
			{
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), L"* miss *"));
			}
		}, nullptr);
		target->ApplyEffect(StatusEffect(damage));
	});

	// Flurry – 1-3 weapon strikes – 15 mana
	Descriptor::TargetFunction flurry_action([&game] (User user, Target target)
	{
		Status<vr::Actor> damage(1, nullptr, [&game,user] (vr::Actor &actor, int duration)
		{
			if (attack_roll(*user, actor))
			{
				auto damage = calculate_damage(*user, actor, Attribute::strength);
				actor.Damage(int(damage));

				// broadcast
				std::wostringstream ss;
				ss << L"-" << damage;
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), ss.str()));
			}
			else
			{
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), L"* miss *"));
			}
		}, nullptr);
		target->ApplyEffect(StatusEffect(damage));
		target->ApplyEffect(StatusEffect(damage));
		target->ApplyEffect(StatusEffect(damage));
	});

	// Aim – 50% weapon damage, restore 5 energy
	Descriptor::TargetFunction aim_action([&game] (User user, Target target)
	{
		Status<vr::Actor> damage(1, nullptr, [&game,user] (vr::Actor &actor, int duration)
		{
			if (attack_roll(*user, actor))
			{
				auto weapon_damage = calculate_damage(*user, actor, Attribute::dexterity);
				int damage = int(std::ceil(weapon_damage * 0.5));
				actor.Damage(damage);

				// broadcast
				std::wostringstream ss;
				ss << L"-" << damage;
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), ss.str()));
			}
			else
			{
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), L"* miss *"));
			}
		}, nullptr);
		user->RestoreEnergy(5);
		target->ApplyEffect(StatusEffect(damage));
	});

	// Precice Shot – 150% weapon damage, 10 energy
	Descriptor::TargetFunction shot_action([&game] (User user, Target target)
	{
		Status<vr::Actor> damage(1, nullptr, [&game,user] (vr::Actor &actor, int duration)
		{
			if (attack_roll(*user, actor))
			{
				auto weapon_damage = calculate_damage(*user, actor, Attribute::dexterity);
				int damage = int(std::ceil(weapon_damage * 1.5));
				actor.Damage(damage);

				// broadcast
				std::wostringstream ss;
				ss << L"-" << damage;
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), ss.str()));
			}
			else
			{
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), L"* miss *"));
			}
		}, nullptr);
		target->ApplyEffect(StatusEffect(damage));
	});

	// Ambush – triple weapon damage if target at full health
	Descriptor::TargetFunction ambush_action([&game] (User user, Target target)
	{
		Status<vr::Actor> damage(1, nullptr, [&game,user] (vr::Actor &actor, int duration)
		{
			if (attack_roll(*user, actor))
			{
				auto weapon_damage = calculate_damage(*user, actor, Attribute::dexterity);
				if (actor.GetCurrentHealth() == actor.GetMaximumHealth())
				{
					weapon_damage *= 3;
				}
				int damage = int(std::ceil(weapon_damage * 0.5));
				actor.Damage(damage);

				// broadcast
				std::wostringstream ss;
				ss << L"-" << damage;
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), ss.str()));
			}
			else
			{
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), L"* miss *"));
			}
		}, nullptr);
		target->ApplyEffect(StatusEffect(damage));
	});

	// Zap – 100% weapon damage 10 mana
	Descriptor::TargetFunction zap_action([&game] (User user, Target target)
	{
		Status<vr::Actor> damage(1, nullptr, [&game,user] (vr::Actor &actor, int duration)
		{
			if (attack_roll(*user, actor))
			{
				auto weapon_damage = calculate_damage(*user, actor, Attribute::intelligence);
				int damage = int(std::ceil(weapon_damage * 1));
				actor.Damage(damage);

				// broadcast
				std::wostringstream ss;
				ss << L"-" << damage;
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), ss.str()));
			}
			else
			{
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), L"* miss *"));
			}
		}, nullptr);
		target->ApplyEffect(StatusEffect(damage));
	});

	// Pain – 20% weapon damage over 20 turns
	Descriptor::TargetFunction pain_action([&game] (User user, Target target)
	{
		Status<vr::Actor> damage(1, nullptr, [&game,user] (vr::Actor &actor, int duration)
		{
			if (attack_roll(*user, actor))
			{
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), L"Pain, pain, pain"));

				// apply dot
				Status<vr::Actor> dot(20, "Pain", [&game,user] (vr::Actor &actor, int duration)
				{
					auto weapon_damage = calculate_damage(*user, actor, Attribute::intelligence);
					int damage = int(std::ceil(weapon_damage * 0.20));
					actor.Damage(damage);

					// broadcast
					std::wostringstream ss;
					ss << L"-" << damage;
					game.Broadcast(actor, Game::Message(Color(1, 1, 0), ss.str()));
				});
				actor.ApplyEffect(StatusEffect(dot));
			}
			else
			{
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), L"* miss *"));
			}
		}, nullptr);
		target->ApplyEffect(StatusEffect(damage));
	});

	// Smite – 150% weapon damage CD 5turns
	Descriptor::TargetFunction smite_action([&game] (User user, Target target)
	{
		Status<vr::Actor> damage(1, nullptr, [&game,user] (vr::Actor &actor, int duration)
		{
			if (attack_roll(*user, actor))
			{
				auto weapon_damage = calculate_damage(*user, actor, Attribute::intelligence);
				int damage = int(std::ceil(weapon_damage * 1.5));
				actor.Damage(damage);

				// broadcast
				std::wostringstream ss;
				ss << L"-" << damage;
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), ss.str()));
			}
			else
			{
				game.Broadcast(actor, Game::Message(Color(1, 1, 0), L"* miss *"));
			}
		}, nullptr);
		target->ApplyEffect(StatusEffect(damage));
	});

	// teleport
	Descriptor::GroundFunction teleport_action([&] (User user, const Point &point)
	{
		vr::Scene *scene = game.GetScene();
		if (scene->IsTraversable(point))
		{
			scene->Move(user, point);
		}
	});
	Descriptor::GroundFunctionCheck teleport_check([&game] (User user, const Point &point)
	{
		return game.GetScene()->IsTraversable(point);
	});

	// Panacea - Heal 70%
	Descriptor::GroundFunction panacea_action([&] (User user, const Point &point)
	{ 
		int hp = user->GetMaximumHealth();
		int heal = (int(std::ceil(hp * 0.7)));
		user->Heal(heal);

		// broadcast
		std::wostringstream ss;
		ss << L"+" << heal;
		game.Broadcast(*user, Game::Message(Color(0, 1, 0), ss.str()));
	});

	// Healing Touch – 50% in 5 turns 20 mana
	Descriptor::GroundFunction healing_action([&] (User user, const Point &point)
	{ 
		Status<vr::Actor> heal
			(5, "Healing Touch",
			[&game] (vr::Actor &actor, int duration)
		{
			int hp = actor.GetMaximumHealth();
			int heal = (int(std::ceil(hp * 0.1)));
			actor.Heal(heal);

			// broadcast
			std::wostringstream ss;
			ss << L"+" << heal;
			game.Broadcast(actor, Game::Message(Color(0, 1, 0), ss.str()));
		});

		user->ApplyEffect(StatusEffect(heal));
	});

	// Flash Heal 30% heal 50 mana
	Descriptor::GroundFunction flash_action([&] (User user, const Point &point)
	{ 
		int hp = user->GetMaximumHealth();
		int heal = (int(std::ceil(hp * 0.3)));
		user->Heal(heal);

		// broadcast
		std::wostringstream ss;
		ss << L"+" << heal;
		game.Broadcast(*user, Game::Message(Color(0, 1, 0), ss.str()));
	});

	// Doom - 500% weapon damage in 5 turns
	Descriptor::TargetFunction doom_action([&] (User user, Target target)
	{ 
		Status<vr::Actor> doom
			(6, "Doom",
			nullptr,
			[&game] (vr::Actor &actor, int duration)
		{
			game.Broadcast(actor, Game::Message(Color(1, 0, 0), L"Doomed"));
		},
			[&game,user] (vr::Actor &actor, int duration)
		{
			int damage = user->GetAttribute(mech::Attribute::damage) + user->GetAttribute(mech::Attribute::strength);
			damage *= 5;

			actor.Damage(damage);
			// broadcast
			std::wostringstream ss;
			ss << L"-" << damage;
			game.Broadcast(actor, Game::Message(Color(1, 1, 0), ss.str()));
		});

		target->ApplyEffect(StatusEffect(doom));
	});

	Descriptor meelee(Attribute::strength, meelee_action, meelee_check);
	meelee.SetText("Meelee", "Do weapon damage.");
	meelee.SetRange("meelee");
	meelee.SetHidden();

	Descriptor power_strike(Attribute::strength, power_strike_action, meelee_check);
	power_strike.SetText("Power Strike", "Do double weapon damage.");
	power_strike.SetRange("meelee");
	power_strike.SetCost(20);

	Descriptor rend(Attribute::strength, rend_action, meelee_check);
	rend.SetText("Rend", "Attack and apply wounds. Target bleeds for 5 turns for total 125% weapon damage.");
	rend.SetRange("meelee");
	rend.SetCost(20);
	rend.SetCooldown(5);

	Descriptor flurry(Attribute::strength, flurry_action, meelee_check);
	flurry.SetText("Flurry", "Perform 1-3 weapon attacks in one turn.");
	flurry.SetRange("meelee");
	flurry.SetCost(15);

	Descriptor aim(Attribute::dexterity, aim_action, hostile_check);
	aim.SetRange("ranged");
	aim.SetText("Aim", "Do half weapon damage, restore 5 energy.");

	Descriptor shot(Attribute::dexterity, shot_action, hostile_check);
	shot.SetText("Precise Shot", "Do 150% weapon damage.");
	shot.SetCost(10);

	Descriptor ambush(Attribute::dexterity, ambush_action, hostile_check);
	ambush.SetText("Ambush!", "Do triple weapon damage if target at full health.");
	ambush.SetRange("ranged");
	ambush.SetCost(10);

	Descriptor zap(Attribute::intelligence, zap_action, hostile_check);
	zap.SetText("Zap", "Zap target.");
	zap.SetRange("ranged");
	zap.SetCost(10);

	Descriptor pain(Attribute::intelligence, pain_action, hostile_check);
	pain.SetText("Pain", "20% weapon damage over 20 turns.");
	pain.SetRange("ranged");

	Descriptor smite(Attribute::intelligence, smite_action, hostile_check);
	smite.SetText("Smite", "Do 150% weapon damage.");
	smite.SetRange("ranged");
	smite.SetCooldown(5);

	Descriptor doom(Attribute::intelligence, doom_action, hostile_check);
	doom.SetText("Doom", "You invoke doom on your opponent, he takes 500% weapon damage in 5 turns.");
	doom.SetRange("ranged");

	Descriptor teleport(Attribute::vitality, teleport_action, teleport_check);
	teleport.SetText("Teleport", "Teleports you to target location.");
	teleport.SetRange("ranged");
	teleport.SetHidden();

	Descriptor panacea(Attribute::vitality, panacea_action, ground_true);
	panacea.SetText("Panacea", "Conjure healing potion from thin air and qaff immediately. Restore 70% health.");
	panacea.SetRange("self");
	panacea.SetCooldown(25);

	Descriptor healing(Attribute::vitality, healing_action, ground_true);
	healing.SetText("Healing Touch", "Heals 50% over 5 turns.");
	healing.SetRange("self");
	healing.SetCooldown(5);
	healing.SetCost(20);

	Descriptor flash(Attribute::vitality, flash_action, ground_true);
	flash.SetText("Flash Heal", "Restore 30% health.");
	flash.SetRange("self");
	flash.SetCost(50);


	_skills.reserve(25);

	// str
	_skills.push_back(meelee);
	_skills.push_back(power_strike);
	_skills.push_back(rend);
	_skills.push_back(flurry);

	// int
	_skills.push_back(zap);
	_skills.push_back(pain);
	_skills.push_back(doom);
	_skills.push_back(smite);

	// dex
	_skills.push_back(aim);
	_skills.push_back(shot);
	_skills.push_back(ambush);

	// vit
	_skills.push_back(teleport);
	_skills.push_back(panacea);
	_skills.push_back(healing);
	_skills.push_back(flash);
}

AbilityDirectory::Descriptor AbilityDirectory::GetDescriptor(int ability_index) const
{
	return _skills.at(ability_index);
}

AbilityDirectory::Descriptor AbilityDirectory::GetDescriptor(std::string name) const
{
	for (const Descriptor &i : _skills)
	{
		if (i.GetName() == name) return i;
	}
	throw std::runtime_error("AbilityDirectory::GetDescriptor(std::string name) const - not found");
}

void AbilityDirectory::EnumAbilities(std::function<bool(int ability_index, const Descriptor &ability)> enum_function) const
{
	int counter = 0;
	for (auto it = _skills.begin(); it != _skills.end(); ++it, ++counter)
	{
		if (it->IsHidden()) continue;

		if (!enum_function(counter, *it)) return;
	}
}
void AbilityDirectory::EnumAbilities(Attribute specialization, std::function<bool(int ability_index, const Descriptor &ability)> enum_function) const
{
	int counter = 0;
	for (auto it = _skills.begin(); it != _skills.end(); ++it, ++counter)
	{
		if (it->IsHidden()) continue;

		if (it->GetSpecialisation() == specialization)
		{
			if (!enum_function(counter, *it)) return;
		}
	}
}