/*****************************************************************//**
 * \file   Timer.hpp
 * \brief  adds a timer object that can be stopped and started
 * 
 * \author 2018t
 * \date   May 2024
 *********************************************************************/

#include <chrono>
#include <iostream>

namespace gep
{
	template <typename TimeMeasure = std::chrono::seconds>
	class Timer
	{
	public:
		Timer() = default;
		~Timer() = default;

		void Start()
		{
			mStartTime = clock_t::now();
		}

		double Stop() const
		{
			return std::chrono::duration_cast<duration_t>(clock_t::now() - mStartTime).count();
		}

		std::string AsString() const
		{
			std::stringstream result;
			result << std::setprecision(6) << Stop();
			return result.str();
		}

		template <typename T>
		friend std::ostream& operator<<(std::ostream& os, const Timer<T>& timer);

	private:
		using clock_t = std::chrono::high_resolution_clock;
		using duration_t = std::chrono::duration<double, typename TimeMeasure::period>;

		std::chrono::time_point<clock_t> mStartTime;
	};

	template <typename TimeMeasure>
	std::ostream& operator<<(std::ostream& os, const Timer<TimeMeasure>& timer)
	{
		os << "Time Elapsed: " << std::setprecision(6) << timer.Stop() << std::endl;

		return os;
	}

}