// Your implementation of the OrderCache...
#include "OrderCache.h"
#include <iostream>
#define VERBOSE 0

OrderCache::OrderCache(void){
	this->_totalOrders = 0;
	this->_totalBuyOrders = 0;
	this->_totalSellOrders = 0;
	return ;
}

OrderCache::~OrderCache(void){
	return ;
}

// we shave off a few nanosecs by inlining the function
void inline OrderCache::addOrder(Order order) {
	this->_mutex.lock();
	this->_orders.emplace(order.orderId(), order);
	this->_totalOrders++;
	order.side().compare(this->_buyLookUpTerm) == 0 ? this->_totalBuyOrders++ : this->_totalSellOrders++;
	this->_mutex.unlock();
}

// TODO: segfault se orderId não existir
void OrderCache::cancelOrder(const std::string& orderId) {
	this->_mutex.lock();
	if (VERBOSE)
		std::cout << "cancelOrder(): " << orderId << std::endl;

	this->_orders.find(orderId)->second.side().compare(this->_buyLookUpTerm) == 0 ? this->_totalBuyOrders-- : this->_totalSellOrders--;
	this->_totalOrders--;
	this->_orders.erase(orderId);
	this->_mutex.unlock();
}

void OrderCache::cancelOrdersForUser(const std::string& user) {
	this->_mutex.lock();
	if (VERBOSE)
		std::cout << "cancelOrdersForUser(): " << user << std::endl;
	//needs optmization here
	for (auto it = this->_orders.begin(); it != this->_orders.end();){
		if (it->second.user() == user) {
			it->second.side().compare(this->_buyLookUpTerm) == 0 ? this->_totalBuyOrders-- : this->_totalSellOrders--;
			this->_totalOrders--;
			it = this->_orders.erase(it);
		}
		else
			++it;
	}

	this->_mutex.unlock();
	return ;
}

// remove all orders in the cache for this security with qty >= minQty
// MELHORAR USO DE MEMORIA!!!
void OrderCache::cancelOrdersForSecIdWithMinimumQty(const std::string& securityId, unsigned int minQty) {
	this->_mutex.lock();
	for (auto it = this->_orders.begin(); it != this->_orders.end();){
		if (it->second.securityId() == securityId && it->second.qty() >= minQty) {
			it->second.side().compare(this->_buyLookUpTerm) == 0 ? this->_totalBuyOrders-- : this->_totalSellOrders--;
			this->_totalOrders--;
			it = this->_orders.erase(it);
		}
		else
			++it;
	}
	this->_mutex.unlock();
}

static inline bool _compareQtyAscending(const Order & a, const Order & b){
	return (a.qty() < b.qty());
}

static inline bool _compareQtyDescending(const Order & a, const Order & b){
	return (a.qty() > b.qty());
}

/*
// <<---------------------------------------------------------------------------

unsigned int OrderCache::getMatchingSizeForSecurity(const std::string& securityId) {
	(void) (securityId);
	std::vector<OrderToMatch> buyOrders;
	std::vector<OrderToMatch> sellOrders;
	// here we preallocate the necessary memmory to improve populating perfomance
	buyOrders.reserve(this->_totalBuyOrders);
	sellOrders.reserve(this->_totalSellOrders);
	unsigned int ret = 0;
	this->_mutex.lock();

	std::for_each(std::execution::par, this->_orders.begin(), this->_orders.end(), [this, &buyOrders, &sellOrders, &securityId](std::pair<const std::string, Order>& it){
		if (it.second.securityId().compare(securityId) != 0){
			;
		}
		else if (it.second.side().compare(this->_buyLookUpTerm) == 0){
			buyOrders.push_back(OrderToMatch(it.second));
		}
		else
			sellOrders.push_back(OrderToMatch(it.second));
	});

	//we sort all stuff enforcing parallelism for better perfomance in high loads
	//note that sorting with descending values will benefit bulkier trades
	std::sort(std::execution::par, buyOrders.begin(), buyOrders.end(), _compareQtyDescending);
	auto sellIt = sellOrders.begin();
	// sort ascending here to have better as to consume samaller sell orders faster to gradually improve perfomance and free memmory;
	std::sort( std::execution::par, sellIt, sellOrders.end(), _compareQtyAscending);

// DEBUG -- prints relevant data
	if (0){
		uint32_t size = 0;
		std::cout << "-----------Printing all SORTED BUY orders------------" << std::endl;
		for (const auto& buyIt : buyOrders) {
			std::cout << buyIt.orderId() << " | " << buyIt.securityId() << " | " << buyIt.side() << " | " << buyIt.user() << " | " << buyIt.company() << " | " << buyIt.qty() << std::endl;
			size++;
		}
		std::cout << "Listed " << size << " entries." << std::endl;
		size = 0;
		std::cout << "-----------Printing all SORTED BUY orders------------" << std::endl;
		while(sellIt != sellOrders.end()){
			std::cout << sellIt->orderId() << " | " << sellIt->securityId() << " | " << sellIt->side() << " | " << sellIt->user() << " | " << sellIt->company() << " | " << sellIt->qty() << std::endl;
			sellIt++;
			size++;
		}
		std::cout << "Listed " << size << " entries." << std::endl;
		sellIt = sellOrders.begin();
	}


	//ATENCAO!!! PRECISAREMOS MELHORAR ESTE ALGORITMO
	if (buyOrders.size() == 0 || sellOrders.size() == 0){
		this->_mutex.unlock();
		return 0;
	}

	// auto buyIt = buyOrders.begin();
	// while (buyIt != buyOrders.end()){
	//     while (sellIt != sellOrders.end()){
	//       if ( (buyIt->company().compare(sellIt->company()) != 0) && buyIt->getQty() && sellIt->getQty()){
	//         if (sellIt->getQty() >= buyIt->getQty()){
	//           sellIt->setQty(sellIt->getQty() - buyIt->getQty());
	//           ret += buyIt->getQty();
	//           buyIt->setQty(0);
	//           break;
	//         }
	//         else{
	//           buyIt->setQty(buyIt->getQty() - sellIt->getQty());
	//           ret += sellIt->getQty();
	//           sellOrders.erase(sellIt);
	//         }
	//       }
	//       else{
	//         sellIt++;
	//       }
	//     }
	//     sellIt = sellOrders.begin();
	//     buyIt++;
	//   }

	auto doMatch = [&sellOrders, &securityId, &ret](OrderToMatch& buyOrder){
		auto sellIt = sellOrders.begin();
		while(sellIt != sellOrders.end()){
			if( (buyOrder.company().compare(sellIt->company()) != 0) && buyOrder.getQty() && sellIt->getQty()){
				if(sellIt->getQty() >= buyOrder.getQty()){
					sellIt->setQty(sellIt->getQty() - buyOrder.getQty());
					ret += buyOrder.getQty();
					buyOrder.setQty(0);
					return ;
				}
				else{
					buyOrder.setQty(buyOrder.getQty() - sellIt->getQty());
					ret += sellIt->getQty();
					sellOrders.erase(sellIt);
				}
			}
			else{
				sellIt++;
			}
		}
	};
	std::for_each(std::execution::par, buyOrders.begin(), buyOrders.end(), doMatch);

	this->_mutex.unlock();
	return ret;
}

// --------------------------------------------------------------------------->> 
*/


/*
// <<---------------------------------------------------------------------------
static void populateCompanyOrders(std::pair<const std::string, Order> &it ,std::unordered_map<std::string, std::deque<OrderToMatch>> &OrdersByCompany){
	auto companyIt = OrdersByCompany.find(it.second.company());
	if (companyIt == OrdersByCompany.end()){ //checks wether the company exists in the map
		//creates a map entry with (company, deque(current order));
		OrdersByCompany.emplace(it.second.company(), std::deque<OrderToMatch>{it.second});
	}
	else{ // adds the order for the current company
		companyIt->second.push_back(it.second);
	}
}

static unsigned int matchOrdersInDeques(std::deque<OrderToMatch> &buyDeque, std::deque<OrderToMatch> &sellDeque){
	unsigned int ret = 0;
	std::deque<OrderToMatch>::iterator buyOrder = buyDeque.begin();
	std::deque<OrderToMatch>::iterator sellOrder = sellDeque.begin();
	while (buyOrder != buyDeque.end()){
		if (buyDeque.size() == 0 || sellDeque.size() == 0)
			break;

		sellOrder = sellDeque.begin();
		while (sellOrder != sellDeque.end()){
			if (buyOrder->getQty() == sellOrder->getQty()){
				ret += buyOrder->getQty();
				buyOrder = buyDeque.erase(buyOrder);
				sellDeque.erase(sellOrder);
				break ;
			}
			else if(buyOrder->getQty() < sellOrder->getQty()){
				ret += buyOrder->getQty();
				sellOrder->setQty(sellOrder->getQty() - buyOrder->getQty());
				buyOrder = buyDeque.erase(buyOrder);
				break;
			}
			else{
				ret += sellOrder->getQty();
				buyOrder->setQty(buyOrder->getQty() - sellOrder->getQty());
				sellOrder = sellDeque.erase(sellOrder);
			}
		}
		if (buyOrder != buyDeque.end()){
			++buyOrder;
		}
	}
	return ret;
}

unsigned int OrderCache::getMatchingSizeForSecurity(const std::string& securityId) {
	
	//                   company          orders
	std::unordered_map<std::string, std::deque<OrderToMatch>> buyOrdersByCompany;
	std::unordered_map<std::string, std::deque<OrderToMatch>> sellOrdersByCompany;
	unsigned int total = 0;

	std::for_each(std::execution::par, this->_orders.begin(), this->_orders.end(), [this, &buyOrdersByCompany, &sellOrdersByCompany, &securityId](std::pair<const std::string, Order>& it){
		if (it.second.securityId().compare(securityId) != 0){
			return ; // does nothing for current entry
		}
		else if (it.second.side().compare(this->_buyLookUpTerm) == 0){ //buyOrders
		
			populateCompanyOrders(it, buyOrdersByCompany);
		}
		else
			populateCompanyOrders(it, sellOrdersByCompany); //sellOrders
	});

	if (0){
		std::cout << "Populate result:" << std::endl;
		for (auto a : buyOrdersByCompany){
			for (auto b: a.second){
				std::cout << b.orderId() << " " << b.securityId() << " " << b.side() << " " << " " << b.qty() << " " << b.user() << " " << b.company() << " " << b.isMatch() << std::endl;
			}
		}
		for (auto a : sellOrdersByCompany){
			for (auto b: a.second){
				std::cout << b.orderId() << " " << b.securityId() << " " << b.side() << " " << " " << b.qty() << " " << b.user() << " " << b.company() << " " << b.isMatch() << std::endl;
			}
		}
		std::cout << "---------------------" << std::endl;
		std::cout << "Starting matches" << std::endl;
	}

	std::unordered_map<std::string, std::deque<OrderToMatch>>::iterator buyMapIt = buyOrdersByCompany.begin();
	std::unordered_map<std::string, std::deque<OrderToMatch>>::iterator sellMapIt = sellOrdersByCompany.begin();
	while (buyMapIt != buyOrdersByCompany.end()){ // para cada empresa comprando
		while (sellMapIt != sellOrdersByCompany.end()){ // para cada empresa vendendo
			if(buyMapIt->first.compare(sellMapIt->first) != 0) // se forem empresas diferentes
				total += matchOrdersInDeques(buyMapIt->second, sellMapIt->second); //comparamos os deques de orders e retornamos os matches
			sellMapIt++;
		}
		sellMapIt = sellOrdersByCompany.begin();
		buyMapIt++;
	}

	return total;
}

// --------------------------------------------------------------------------->> 
*/
//secIds -> companies -> buy/sell

//
unsigned int OrderCache::getMatchingSizeForSecurity(const std::string& securityId) {
	std::unordered_map<std::string, uint32_t> buy;
	std::unordered_map<std::string, uint32_t> sell;
	unsigned int total = 0;

	for (auto order : this->_orders) {
		if(order.second.securityId().compare(securityId) == 0){
			if (order.second.side().compare(this->_buyLookUpTerm) == 0) {
				auto company = buy.find(order.second.company());
				if (company == buy.end()){
					buy.emplace(order.second.company(), order.second.qty());
				}
				else {
					company->second += order.second.qty();
				}
			}
			else {
				auto company = sell.find(order.second.company());
				if (company == sell.end()){
					sell.emplace(order.second.company(), order.second.qty());
				}
				else{
					company->second += order.second.qty();
				}
			}
		}
	}

	if(0){
		for (auto a : buy){
			std::cout << "BUY | " << a.first << ": " << a.second << std::endl;
		}
		for (auto a : sell){
			std::cout << "SELL | " << a.first << ": " << a.second << std::endl;
		}
	}

	for (auto &buyIt : buy){
		for (auto &sellIt : sell){
			std::cout << "B: [" << buyIt.first << " | " << buyIt.second << "]\t" << "S: [" << sellIt.first << " | " << sellIt.second << "]" << std::endl;
			if (buyIt.second == 0)
				break;
			if (buyIt.first.compare(sellIt.first) == 0 || sellIt.second == 0)
				continue;
			if (buyIt.second == sellIt.second){
				total += buyIt.second;
				buyIt.second = 0;
				sellIt.second = 0;
				std::cout << "-> B: [" << buyIt.first << " | " << buyIt.second << "]\t" << "S: [" << sellIt.first << " | " << sellIt.second << "]" << std::endl;
				break;
			}
			else if (buyIt.second < sellIt.second){
				total += buyIt.second;
				sellIt.second -= buyIt.second;
				buyIt.second = 0;
				std::cout << "-> B: [" << buyIt.first << " | " << buyIt.second << "]\t" << "S: [" << sellIt.first << " | " << sellIt.second << "]" << std::endl;
				break;
			}
			else if (buyIt.second > sellIt.second){
				total += sellIt.second;
				buyIt.second -= sellIt.second;
				sellIt.second = 0;
				std::cout << "-> B: [" << buyIt.first << " | " << buyIt.second << "]\t" << "S: [" << sellIt.first << " | " << sellIt.second << "]" << std::endl;
			}
		}

	}
	if(1){
		for (auto a : buy){
			std::cout << "BUY | " << a.first << ": " << a.second << std::endl;
		}
		for (auto a : sell){
			std::cout << "SELL | " << a.first << ": " << a.second << std::endl;
		}
	}
	if (0)
		std::cout << "TOTAL: " << total << std::endl;
	return total;
}
//
void OrderCache::assertOrderNumbers(void){
	uint32_t lTotalOrders = this->_totalBuyOrders + this->_totalSellOrders;
	assert(lTotalOrders == this->_totalOrders);
	lTotalOrders = (uint32_t)(this->_orders.size());
	assert(lTotalOrders == this->_totalOrders);
}

std::vector<Order> OrderCache::getAllOrders() const {
	if (VERBOSE)
		std::cout << "getAllOrders()" << std::endl;
	std::vector<Order> ret;
	ret.reserve(this->_totalOrders);
	this->_mutex.lock();
	for (const auto& entry : this->_orders) {
		ret.push_back(entry.second);
	}
	this->_mutex.unlock();
	return ret;
}

