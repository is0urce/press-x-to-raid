// name: Timer.h
// type: c++ header
// desc: class declaration & implementation

// time track

#pragma once

namespace px
{
	class Timer
	{
	private:
		double _freq;
		__int64 _start;

	public:
		Timer() : _start(0), _freq(-1.0) {}
		~Timer() {}

		void Start()
		{
			LARGE_INTEGER li;
			if(!QueryPerformanceFrequency(&li))
			{
				throw std::runtime_error("Timer::StartCounter() QueryPerformanceFrequency fails");
			}

			_freq = double(li.QuadPart);// / 1000.0;

			QueryPerformanceCounter(&li);
			_start = li.QuadPart;
		}

		double GetCounter() const
		{
			LARGE_INTEGER li;
			QueryPerformanceCounter(&li);
			return double(li.QuadPart - _start) / _freq;
		}
	};
}