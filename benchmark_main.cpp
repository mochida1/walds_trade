#include "Benchmark.hpp"
#include "OrderCache.h"
#include "OrderFactory.hpp"

double bmWait1kNanosecsHRC(void){
	BenchmarkHRC bm;
	bm.start();
	std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
	bm.end();

	std::cout << "Time to wait for 1000 nanosecs: " << bm.getLast().count() << std::endl;
	return bm.getLast().count();
}

double bmWait1kNanosecsCPS(void) {
    BenchmarkCPS bm;
    bm.start();
    std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
    bm.end();
    
	std::cout << "Time to wait for 1000 nanosecs (CPS): " << bm.getLast() << std::endl;
    return bm.getLast();
}

double bmSingleOperation(void){
	BenchmarkHRC bm;
	int a = 0;
	bm.start();
	a = 1;
	bm.end();

	std::cout << "Time for single operation: " << bm.getLast().count() << std::endl;
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

	std::cout << "Time to ADD SINGLE ORDER: " << bm.getLast().count() << std::endl;
	return bm.getLast().count();
}

double bmAddOrder(uint32_t times_to_run){
	OrderCache cache;
	OrderFactory OF;
	BenchmarkHRC bm;
	double result = 0;

	for (uint32_t i = 0; i < times_to_run; i++){
		Order singleOrder = OF.singleOrderGenerator();
		bm.start();
		cache.addOrder(singleOrder);
		bm.end();
		result += bm.getLast().count();
	}
	std::cout << "Time to ADD " << times_to_run << " ORDERS: " << result << " | avarage: " << result/times_to_run << std::endl;
	return result;
}

double bmCancelOrder(uint32_t times_to_run){
	OrderCache cache;
	OrderFactory OF;
	BenchmarkHRC bm;
	double totalTime = 0;

	OF.batchOrderGenerator(times_to_run, cache);
	for (uint32_t i = 0; i < times_to_run; i++){
		std::string orderId = "OrdId" + std::to_string(i+1);
		bm.start();
		cache.cancelOrder(orderId);
		bm.end();
		totalTime += bm.getLast().count();
	}
	std::cout << "Time to CANCEL " << std::to_string(times_to_run) << " orders: " << totalTime << " | average: " << totalTime/times_to_run << std::endl;
	return totalTime;
}

double bmCancelOrderForUser(uint32_t total_orders, uint32_t total_users){
	OrderCache cache;
	BenchmarkHRC bm;
	double totalTime = 0;
	std::string user_to_delete = "User1";

	for (uint32_t i = 0; i < total_orders; i++){
		std::string orderId = "OrdId" + std::to_string(i+1);
		std::string user = "User" + std::to_string((i % total_users) + 1);
		cache.addOrder(Order{orderId, "SecId1", "Buy", 300, user, "Company2"});
	}
	bm.start();
	cache.cancelOrdersForUser("User1");
	bm.end();
	totalTime += bm.getLast().count();
	std::cout << "Time to CANCEL " << std::to_string(total_orders/total_users) << " orders for " + user_to_delete + " in " << total_orders << ": " << totalTime << " | average: " << totalTime/total_orders << std::endl;
	return totalTime;
}

#define NANOSECS 1e-9

//   void addOrder(Order order) override;
//   void cancelOrder(const std::string& orderId) override;
//   void cancelOrdersForUser(const std::string& user) override;
//   void cancelOrdersForSecIdWithMinimumQty(const std::string& securityId, unsigned int minQty) override;
//   unsigned int getMatchingSizeForSecurity(const std::string& securityId) override;
//   std::vector<Order> getAllOrders() const override;

int main(void){
	std::cout << std::fixed << std::setprecision(9) << std::left;

	double result = 0;
	uint32_t times_to_run = 1000000;
	bmWait1kNanosecsHRC();
	bmWait1kNanosecsCPS();
	bmSingleOperation();
	result += bmAddOrder();
	result += bmAddOrder(times_to_run);
	result += bmCancelOrder(times_to_run);
	result += bmCancelOrderForUser(10000000, 5);
	

	std::cout << "TOTAL time spent on OrderCache methods: " << result << std::endl;
}