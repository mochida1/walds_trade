#include "OrderCache.h"
#include "OrderFactory.hpp"

void TEST_1(void){
	std::cout << "\nTEST_1" << std::endl;
	OrderCache cache;
	OrderFactory orderFact;
	orderFact.printSettings();
	orderFact.batchOrderGenerator(99, cache);
	cache.addOrder(orderFact.singleOrderGenerator());
	cache.assertOrderNumbers();
	cache.cancelOrdersForUser("User1");
	cache.assertOrderNumbers();
	// std::vector<Order> orderVector = cache.getAllOrders();
	// OrderFactory::printOrderContainer(orderVector);
	// std::cout << "\ncache.getMatchingSizeForSecurity(\"SecId2\");" << std::endl;
	cache.getMatchingSizeForSecurity("SecId2");
}

void TEST_2(void){
	std::cout << "\nTEST_2" << std::endl;
	OrderCache cache;
	cache.addOrder(Order{"OrdId1", "SecId1", "Buy", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId2", "SecId1", "Buy", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId3", "SecId1", "Buy", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId4", "SecId1", "Buy", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId5", "SecId1", "Sell", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId6", "SecId1", "Sell", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId7", "SecId1", "Sell", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId8", "SecId1", "Sell", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId9", "SecId2", "Sell", 1000, "User1", "CompanyB"});
	cache.addOrder(Order{"OrdId10", "SecId2", "Sell", 1000, "User1", "CompanyC"});
	unsigned int ret = cache.getMatchingSizeForSecurity("SecId1");
	std::cout << "SizeForSec: " << ret << std::endl;
	assert (ret == 0);
}

void TEST_3(void){
	std::cout << "\nTEST_3" << std::endl;
	OrderCache cache;
	cache.addOrder(Order{"OrdId1", "SecId1", "Buy", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId2", "SecId1", "Buy", 1000, "User1", "CompanyB"});
	cache.addOrder(Order{"OrdId3", "SecId1", "Buy", 1000, "User1", "CompanyC"});
	cache.addOrder(Order{"OrdId4", "SecId1", "Buy", 1000, "User1", "CompanyD"});
	cache.addOrder(Order{"OrdId5", "SecId1", "Sell", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId6", "SecId1", "Sell", 1000, "User1", "CompanyB"});
	cache.addOrder(Order{"OrdId7", "SecId1", "Sell", 1000, "User1", "CompanyC"});
	cache.addOrder(Order{"OrdId8", "SecId1", "Sell", 1000, "User1", "CompanyD"});
	unsigned int ret = cache.getMatchingSizeForSecurity("SecId1");
	std::cout << "SizeForSec: " << ret << std::endl;
	assert (ret == 4000);
}

void TEST_4(void){
	std::cout << "\nTEST_4" << std::endl;
	OrderCache cache;
	cache.addOrder(Order{"OrdId1", "SecId1", "Buy", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId2", "SecId1", "Buy", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId3", "SecId1", "Buy", 1000, "User1", "CompanyB"});
	cache.addOrder(Order{"OrdId4", "SecId1", "Buy", 1000, "User1", "CompanyB"});
	cache.addOrder(Order{"OrdId5", "SecId1", "Sell", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId6", "SecId1", "Sell", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId7", "SecId1", "Sell", 1000, "User1", "CompanyB"});
	cache.addOrder(Order{"OrdId8", "SecId1", "Sell", 1000, "User1", "CompanyB"});
	unsigned int ret = cache.getMatchingSizeForSecurity("SecId1");
	std::cout << "SizeForSec: " << ret << std::endl;
	assert (ret == 4000);
}

void TEST_5(void){ // buy total > sell total
	std::cout << "\nTEST_5" << std::endl;
	OrderCache cache;
	cache.addOrder(Order{"OrdId1", "SecId1", "Buy", 1100, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId2", "SecId1", "Buy", 1200, "User1", "CompanyB"});
	cache.addOrder(Order{"OrdId3", "SecId1", "Buy", 1300, "User1", "CompanyC"});
	cache.addOrder(Order{"OrdId4", "SecId1", "Buy", 1400, "User1", "CompanyD"});
	cache.addOrder(Order{"OrdId5", "SecId1", "Sell", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId6", "SecId1", "Sell", 1000, "User1", "CompanyB"});
	cache.addOrder(Order{"OrdId7", "SecId1", "Sell", 1000, "User1", "CompanyC"});
	cache.addOrder(Order{"OrdId8", "SecId1", "Sell", 1000, "User1", "CompanyD"});
	unsigned int ret = cache.getMatchingSizeForSecurity("SecId1");
	std::cout << "SizeForSec: " << ret << std::endl;
	assert (ret == 4000);
}

void TEST_6(void){ // buy total > sell total
	std::cout << "\nTEST_6" << std::endl;
	OrderCache cache;
	cache.addOrder(Order{"OrdId1", "SecId1", "Buy", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId2", "SecId1", "Buy", 1000, "User1", "CompanyB"});
	cache.addOrder(Order{"OrdId3", "SecId1", "Buy", 1000, "User1", "CompanyC"});
	cache.addOrder(Order{"OrdId4", "SecId1", "Buy", 1000, "User1", "CompanyD"});
	cache.addOrder(Order{"OrdId5", "SecId1", "Sell", 1100, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId6", "SecId1", "Sell", 1200, "User1", "CompanyB"});
	cache.addOrder(Order{"OrdId7", "SecId1", "Sell", 1300, "User1", "CompanyC"});
	cache.addOrder(Order{"OrdId8", "SecId1", "Sell", 1400, "User1", "CompanyD"});
	unsigned int ret = cache.getMatchingSizeForSecurity("SecId1");
	std::cout << "SizeForSec: " << ret << std::endl;
	assert (ret == 4000);
}

void TEST_7(void){ // buy total > sell total
	std::cout << "\nTEST_7" << std::endl;
	OrderCache cache;
	cache.addOrder(Order{"OrdId1", "SecId1", "Buy", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId2", "SecId1", "Buy", 1000, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId3", "SecId1", "Buy", 1000, "User1", "CompanyC"});
	cache.addOrder(Order{"OrdId4", "SecId1", "Buy", 1000, "User1", "CompanyD"});
	cache.addOrder(Order{"OrdId5", "SecId1", "Sell", 1100, "User1", "CompanyA"});
	cache.addOrder(Order{"OrdId6", "SecId1", "Sell", 1200, "User1", "CompanyB"});
	cache.addOrder(Order{"OrdId7", "SecId1", "Sell", 1300, "User1", "CompanyC"});
	cache.addOrder(Order{"OrdId8", "SecId1", "Sell", 1400, "User1", "CompanyD"});

	/*
	A: 2000	| 1100 :: 3900
	B: 0	| 1200 :: 3800
	C: 1000	| 1300 :: 3700
	D: 1000	| 1400 :: 3600
	*/
	// id1(0) - id6(200) -> 1000	(A:B)
	// id2(800) - id6(0) -> 200		(A:B)
	// id2(0) - id7(500) -> 800		(A:C)
	// id3(0) - id8(400) -> 1000	(C:D)
	// id4(500) - id7(0) -> 500		(D:C)
	// id4(500) - id5(500)-> 500	(D:A)
	unsigned int ret = cache.getMatchingSizeForSecurity("SecId1");
	std::cout << "SizeForSec: " << ret << std::endl;
	assert (ret == 4000);
}

int main (void)
{
	// TEST_1();
	TEST_2();
	TEST_3();
	TEST_4();
	TEST_5();
	TEST_6();
	TEST_7();

	return 0;
}