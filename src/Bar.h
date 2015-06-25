// name: Bar.h
// type: c++ header
// desc: template class declaration & implementation
// auth: is0urce

// resource bar

#pragma once

namespace px
{
	template <typename _T>
	struct Bar
	{
		// attributes
	private:
		_T _current;
		_T _max;

		// ctor & dtor
	public:
		Bar()
		{
			Init(0, 0);
		}
		Bar(int points)
		{
			Init(points, points);
		}
		Bar(int current, int max)
		{
			Init(current, max);
		}
	private:
		void Init(_T current, _T max)
		{
			_max = max;
			_current = std::min<_T>(current, _max);
		}

		// methods
	public:
		void SetValues(int current_and_max)
		{
			Init(current_and_max, current_and_max);
		}
		void SetValues(int current, int max)
		{
			Init(current, max);
		}
		int GetCurrent() const
		{
			return _current;
		}
		int GetMaximum() const
		{
			return _max;
		}
		void Restore(_T magnitude)
		{
			if (magnitude < 0) throw std::runtime_error("Bar<T>::Restore(_T magnitude) magnitude < 0");
			_current = std::min<_T>(_current + magnitude, _max);
		}
		void Spend(_T magnitude)
		{
			if (magnitude < 0) throw std::runtime_error("Bar<T>::Spend(_T magnitude) magnitude < 0");
			_current -= magnitude;
		}
		void SetCurrent(_T current)
		{
			_current = std::min<_T>(current, _max);
		}
		void SetMaximum(_T max)
		{
			_max = max;
			_current = std::min<_T>(_current, _max);
		}
		bool IsEmpty() const
		{
			return _current <= 0;
		}
		template <typename _C>
		Bar<_C> Cast() const
		{
			return Bar<_C>((_C)_current, (_C)_max);
		}
	};

}