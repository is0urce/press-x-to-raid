// name: Statistics.h
// type: c++
// desc: class header
// auth: is0urce

#pragma once

namespace px
{
	namespace mech
	{
		enum class Attribute : int;

		class Statistics
		{
		private:
			std::vector<int> _values;

		public:
			Statistics();
			~Statistics();

		public:
			bool IsValid(Attribute stat);
			int GetValue(Attribute stat) const;
			void Clear();
			void Enhance(Attribute stat, int modify);
		};
	}
}
