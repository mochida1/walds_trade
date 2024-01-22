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
	// std::vector<Order> orderVector = cache.getAllOrders();
	// OrderFactory::printOrderContainer(orderVector);
	std::cout << "\ncache.getMatchingSizeForSecurity(\"SecId2\");" << std::endl;
	cache.getMatchingSizeForSecurity("SecId2");
	return 0;
}