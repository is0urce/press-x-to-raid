// name: Enhancement.h
// type: c++
// desc: class header
// auth: is0urce

#pragma once

namespace px
{
	namespace mech
	{
		enum class Attribute : int;

		class Statistics;

		class Enhancement
		{
		private:
			Attribute _index;
			int _modify;

		public:
			Enhancement(Attribute stat, int mod);
			~Enhancement();

		public:
			void Enhance(Statistics &statistics) const;
			Attribute GetAttribute() const;
			int GetValue() const;
		};
	}
}