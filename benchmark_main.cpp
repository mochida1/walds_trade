#include "Benchmark.hpp"
#include "OrderCache.h"
#include "OrderFactory.hpp"

double bmWait1kNanosecsHRC(void){
	BenchmarkHRC bm;
	bm.start();
	std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
	bm.end();
	return bm.getLast().count();
}

double bmWait1kNanosecsCPS(void) {
    BenchmarkCPS bm;
    bm.start();
    std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
    bm.end();
    
    std::cout << std::fixed << std::setprecision(9);
    return bm.getLast();
}

double bmSingleOperation(void){
	BenchmarkHRC bm;
	int a = 0;
	bm.start();
	a = 1;
	bm.end();
	return bm.getLast().count();
}

double bmAddOrder(void){
	BenchmarkHRC bm;
	OrderFactory OF;
	Order singleOrder(OF.singleOrderGenerator());
	OrderCache cache;
	bm.start();
	cache.addOrder(singleOrder);
	bm.end();
	return bm.getLast().count();
}

#define NANOSECS 1e-9

int main(void){
	std::cout << std::fixed << std::setprecision(9) << std::left;
	OrderFactory orderFactory;
	std::vector<Order> orderVect;
	std::cout << "Time to wait for 1000 nanosecs (HRC): " << bmWait1kNanosecsHRC() << std::endl;
	std::cout << "Time to wait for 1000 nanosecs (CPS): " << bmWait1kNanosecsCPS() << std::endl;
	std::cout << "Time for single operation (HRC): " << bmSingleOperation() << std::endl;
	std::cout << "Time to ADD SINGLE ORDER (HRC): " << bmAddOrder() << std::endl;
	double average = 0;
	uint32_t times_to_run = 1000000;
	for (uint32_t i=0; i < times_to_run; i++){
		average += bmAddOrder();
	}
	std::cout << "Time to ADD 1m ORDERS: " << average << " | avarage: " << average/times_to_run << std::endl;

}