#pragma once

#include <chrono>
#include <iostream>

class DurationChecker
{
private:
	std::chrono::high_resolution_clock::time_point	m_start, m_end;
	std::chrono::nanoseconds						m_duration;

public:
	DurationChecker()
	{
		m_start = std::chrono::high_resolution_clock::now();
	}
	virtual ~DurationChecker()
	{
		m_end = std::chrono::high_resolution_clock::now();
		m_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(m_end - m_start);
		//printf("duration : %lli \n", m_duration.count());
		std::cout << "duration : " << m_duration.count() << std::endl;
	}
};