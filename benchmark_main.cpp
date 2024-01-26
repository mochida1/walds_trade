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

double bmWait1SecHRC(void){
	BenchmarkHRC bm;
	bm.start();
	std::this_thread::sleep_for(std::chrono::seconds(1));
	bm.end();

	std::cout << "Time to wait for 1 second: " << bm.getLast().count() << std::endl;
	return bm.getLast().count();
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

double bmCancelOrdersForSecIdWithMinimumQty(uint32_t totalOrders, uint32_t total_IDs){
	OrderCache cache;
	BenchmarkHRC bm;
	double totalTime = 0;
	std::string idToDeleteFrom = "secId1";
	uint32_t minQty = 500;
	uint32_t totalOrdersForSecId = 0;
	uint32_t totalOrdersToRemove = 0;
	uint32_t secIdSerial = 0;
	uint32_t qtySerial = 0;
	std::string orderId;
	std::string secId;

	for (uint32_t i = 0; i < totalOrders; i++){
		secIdSerial = (i % total_IDs) + 1;
		orderId = "OrdId" + std::to_string(i+1);
		secId = "SecId" + std::to_string(secIdSerial);
		qtySerial = ((i+1)*100)%10000;
		if (secIdSerial == 1){
			totalOrdersForSecId++;
			if (qtySerial < minQty)
				totalOrdersToRemove++;
		}
		cache.addOrder(Order{orderId, secId, "Buy", qtySerial, "User1", "Company1"});
	}
	bm.start();
	cache.cancelOrdersForSecIdWithMinimumQty(idToDeleteFrom, minQty);
	bm.end();
	totalTime += bm.getLast().count();
	std::cout << "Time to CANCEL " << std::to_string(totalOrdersToRemove) << " orders for " + idToDeleteFrom + " in " << totalOrdersForSecId << ": " << totalTime << " | average: " << totalTime/totalOrdersToRemove << std::endl;
	return totalTime;
}

double bmGetAllOrders(uint32_t totalOrders){
	OrderCache cache;
	BenchmarkHRC bm;
	OrderFactory OF;
	Order singleOrder(OF.singleOrderGenerator());
	double result;

	for (uint32_t i = 0; i < totalOrders; i++){
		singleOrder = OF.singleOrderGenerator();
		cache.addOrder(singleOrder);
	}

	std::vector<Order> allOrders;
	bm.start();
	allOrders = cache.getAllOrders();
	bm.end();
	result += bm.getLast().count();
	std::cout << "TOTAL time spent for getAllCounters at " << totalOrders << " orders: " << result << std::endl;
	return result;
}

double bmGetMatchingSizeForSecurity(std::string file){
	OrderCache cache;
	BenchmarkHRC bm;
	OrderFactory OF;
	Order singleOrder(OF.singleOrderGenerator());
	std::string secID("SecId1");
	double result;
	uint32_t totalMatches = 0;

	OF.batchOrderFromFile(file, cache);

	bm.start();
	totalMatches = cache.getMatchingSizeForSecurity(secID);
	bm.end();
	result += bm.getLast().count();
	std::cout << "TOTAL time spent for getMatchingSizeForSecurity("+ secID +") to find " << totalMatches << " QTY matches from " << file << ": " << result << std::endl;
	return result;
}

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

	// bmWait1kNanosecsHRC();
	// bmWait1SecHRC();
	// bmWait1kNanosecsCPS();
	// bmSingleOperation();
	// result += bmAddOrder();
	// result += bmAddOrder(times_to_run);
	// result += bmCancelOrder(times_to_run);
	// result += bmCancelOrderForUser(10000000, 5);
	// result += bmCancelOrdersForSecIdWithMinimumQty(10000000, 5);
	// result += bmGetAllOrders(1000000);
	// result += bmGetMatchingSizeForSecurity();

	/* run these lines only if you need a new file with wich to benchmark /*
	// OrderFactory OF;
	// std::string file = OF.createBenchmarkFile(1000000, "bmFile.csv");
	// std::cout << "crated file: " << file << std::endl;
	/* -------------------------------------------------------------------*/
	std::string file("bmFile.csv");
	result = bmGetMatchingSizeForSecurity(file);

	std::cout << "TOTAL time spent on OrderCache methods: " << result << std::endl;
}