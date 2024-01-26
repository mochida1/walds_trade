#include "OrderCache.h"
#include "OrderFactory.hpp"

int main (void)
{
	OrderCache cache;
	OrderFactory orderFact;
	orderFact.printSettings();
	orderFact.batchOrderGenerator(99, cache);
	cache.addOrder(orderFact.singleOrderGenerator());
	cache.assertOrderNumbers();
	cache.cancelOrdersForUser("User1");
	cache.assertOrderNumbers();
	std::cout << "\ncache.getMatchingSizeForSecurity(\"SecId2\"): ";
	std::cout << cache.getMatchingSizeForSecurity("SecId2") << std::endl;
	return 0;
}