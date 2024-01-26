#include "Benchmark.hpp"

BenchmarkHRC::BenchmarkHRC(void){
	return ;
}

void BenchmarkHRC::start(void){
	this->_start = std::chrono::high_resolution_clock::now();
}

void BenchmarkHRC::end(void){
	this->_end = std::chrono::high_resolution_clock::now();
}

std::chrono::duration<double> BenchmarkHRC::getLast(void) const{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(this->_end - this->_start);
}

