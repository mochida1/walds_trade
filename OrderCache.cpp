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
  std::sort( std::execution::par, sellIt, sellOrders.end(), _compareQtyDescending);

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
  auto buyIt = buyOrders.begin();
  while (buyIt != buyOrders.end()){
      while (sellIt != sellOrders.end()){
        if ( (buyIt->company().compare(sellIt->company()) != 0) && buyIt->getQty() && sellIt->getQty()){
          if (sellIt->getQty() >= buyIt->getQty()){
            sellIt->setQty(sellIt->getQty() - buyIt->getQty());
            ret += buyIt->getQty();
            buyIt->setQty(0);
            break;
          }
          else{
            buyIt->setQty(buyIt->getQty() - sellIt->getQty());
            ret += sellIt->getQty();
            sellOrders.erase(sellIt);
          }
        }
        else{
          sellIt++;
        }
      }
      sellIt = sellOrders.begin();
      buyIt++;
    }

  // std::for_each(std::execution::par, buyOrders.begin(), buyOrders.end(), [&sellOrders, &securityId, &ret](std::vector<OrderToMatch>::iterator fBuyIt){
  //   sellIt = sellOrders.begin();
  //   while (sellIt != sellOrders.end()){
  //     if ( (buyIt->company().compare(sellIt->company()) != 0) && fBuyIt->getQty() && sellIt->getQty()){
  //       if (sellIt->getQty() >= fBuyIt->getQty()){
  //         sellIt->setQty(sellIt->getQty() - fBuyIt->getQty());
  //         ret += fBuyIt->getQty();
  //         fBuyIt->setQty(0);
  //         break;
  //       }
  //       else{
  //         fBuyIt->setQty(fBuyIt->getQty() - sellIt->getQty());
  //         ret += sellIt->getQty();
  //         sellIt->setQty(0);
  //       }
  //     }
  //     else{
  //       sellIt++;
  //     }
  //   }
  // });

  this->_mutex.unlock();
  return ret;
}

void OrderCache::assertOrderNumbers(void){
  uint32_t lTotalOrders = this->_totalBuyOrders + this->_totalSellOrders;
  assert(lTotalOrders == this->_totalOrders);
  lTotalOrders = (uint32_t)(this->_orders.size());
  assert(lTotalOrders == this->_totalOrders);
}
// este cara está tomando segfault quando lida com 
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

