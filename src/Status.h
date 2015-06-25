// name: Status.h
// type: c++ header
// desc: class declaration
// auth: is0urce

#pragma once

namespace px
{
	namespace mech
	{
		template <typename _T>
		class Status
		{
		public:
			typedef _T& _R;
			typedef std::function<void(_R, int duration)> EffectDelegate;

		private:
			int _duration;
			EffectDelegate _apply, _expire, _tick;
			std::string _name;

		public:
			Status(int duration, EffectDelegate tick)
				: _duration(duration), _tick(tick) {}
			Status(int duration, EffectDelegate tick, EffectDelegate apply, EffectDelegate expire)
				: _duration(duration), _tick(tick), _apply(apply), _expire(expire) {}
			Status(int duration, std::string name, EffectDelegate tick)
				: _duration(duration), _tick(tick), _name(name) {}
			Status(int duration, std::string name, EffectDelegate tick, EffectDelegate apply, EffectDelegate expire)
				: _duration(duration), _tick(tick), _apply(apply), _expire(expire), _name(name) {}
			~Status() {}

		private:
			void Init(std::string name, int duration, EffectDelegate tick, EffectDelegate apply, EffectDelegate remove) { _name = name; _duration = duration; _tick = tick; _apply = apply; _expire = expire; }

		public:
			int GetDuration() { return _duration; }
			bool IsExpired() { return _duration <= 0; }

			void OnApply(_R actor) { if (_apply) _apply(actor, _duration); }
			void OnExpire(_R actor) { if (_expire) _expire(actor, _duration); }
			void OnTick(_R actor) { if (!IsExpired() && _tick) _tick(actor, _duration); --_duration; }
			void OnTick(_R actor, int delta) { for (int i = 0; i < delta; ++i) OnTick(actor); }

			void SetName(std::string name) { _name = name; }
			std::string GetName() const { std::stringstream ss; ss << _name << " (" << _duration << ")"; return ss.str(); }
		};
	}
}