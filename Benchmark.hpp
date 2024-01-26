#pragma once
#ifndef BENCHMARK_HPP
# define BENCHMARK_HPP
#include <chrono>
#include <iostream>
#include <iomanip>
#include <thread>
#include <ctime>
#include <thread>

class BenchmarkHRC {
public:
	BenchmarkHRC(void);
	void start(void);
	void end(void);
	std::chrono::duration<double> getLast(void) const;
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _start;
	std::chrono::time_point<std::chrono::high_resolution_clock> _end;
};

class BenchmarkCPS {
public:
    void start() {
        start_time = std::clock();
    }

    void end() {
        end_time = std::clock();
    }

    double getLast(){
		double ret = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC;
        return ret;
    }

private:
    std::clock_t start_time;
    std::clock_t end_time;
};

#endif //BENCHMAARK_HPP