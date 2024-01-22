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

void OrderCache::addOrder(Order order) {
  this->_mutex.lock();
  this->_orders.emplace(order.orderId(), order);
  this->_totalOrders++;
  order.side().compare(this->_buyLookUpTerm) == 0 ? this->_totalBuyOrders++ : this->_totalSellOrders++;
  this->_mutex.unlock();
}

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

// - Order matching rules for getMatchingSizeForSecurity()
//   - Your implementation of getMatchingSizeForSecurity() should give the total qty that can match for a security id
//   - Can only match orders with the same security id
//   - Can only match a Buy order with a Sell order
//   - Buy order can match against multiple Sell orders (and vice versa)
//     - eg a security id "ABCD" has
//       Buy  order with qty 10000
//       Sell order with qty  2000
//       Sell order with qty  1000
//     - security id "ABCD" has a total match of 3000. The Buy order's qty is big
//       enough to match against both Sell orders and still has 7000 remaining
//   - Any order quantity already allocated to a match cannot be reused as a match
//     against a different order (eg the qty 3000 matched above for security id
//     "ABCD" example)
//   - Some orders may not match entirely or at all
//   - Users in the same company cannot match against each other
/*
  std::string orderId() const    { return m_orderId; }
  std::string securityId() const { return m_securityId; }
  std::string side() const       { return m_side; }
  std::string user() const       { return m_user; }
  std::string company() const    { return m_company; }
  unsigned int qty() const       { return m_qty; }
*/
//  OrdId8 SecId2 Sell 5000 User8 CompanyE

// criar uma classe que herde de Order, com um novo membro bool para saber se a order já teve match.
// pegar todas Order com securityId's iguais ao parâmetro e dividí-las em dois grupos: uma de buy e outra de sell
// fazer o sort delas por qty
// pensar num jeito de otimizar os matches

static bool _compareQtyAscending(const Order & a, const Order & b){
  return (a.qty() < b.qty());
}

static bool _compareQtyDescending(const Order & a, const Order & b){
  return (a.qty() > b.qty());
}

unsigned int OrderCache::getMatchingSizeForSecurity(const std::string& securityId) {
  (void) (securityId);
  std::vector<Order> buyOrders;
  std::vector<Order> sellOrders;
  // here we preallocate the necessary memmory to improve populating perfomance
  buyOrders.reserve(this->_totalBuyOrders);
  sellOrders.reserve(this->_totalSellOrders);
  unsigned int ret = 0;
  this->_mutex.lock();

  std::for_each(std::execution::par, this->_orders.begin(), this->_orders.end(), [this, &buyOrders, &sellOrders](std::pair<const std::string, Order>& it){
    if (it.second.side().compare(this->_buyLookUpTerm) == 0){
      buyOrders.push_back(it.second);
    }
    else
      sellOrders.push_back(it.second);
  });

  //we sort all stuff enforcing parallelism for better perfomance in high loads
  //note that sorting with descending values will benefit bulkier trades
  std::sort(std::execution::par, buyOrders.begin(), buyOrders.end(), _compareQtyDescending);
  auto sellIt = sellOrders.begin();
  std::sort( std::execution::par, sellIt, sellOrders.end(), _compareQtyDescending);

// DEBUG -- prints relevant data
  if (1){
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
// verificamos os melhores valores de venda para serem adaptados em qty menores
// existe um caveat aqui: seria necessário verificar os melhores matches para conseguir o volume otimo de qty para matches
  for (const auto& buyIt : buyOrders) {
      if (buyIt.company().compare(sellIt->company()) != 0 && buyIt.user().compare(sellIt->user()) != 0 && buyIt.qty() <= sellIt->qty()){
        //fazemos o match
        //se o match deu bom, damos erase em ambos os elementos para que não sejam mais usados.
        //avançamos sellIt, uma vez que o de buyIt será avançado automaticamente.
      }
    }

  this->_mutex.unlock();
  return ret;
}

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
  this->_mutex.lock();
  for (const auto& entry : this->_orders) {
    ret.push_back(entry.second);
  }
  this->_mutex.unlock();
  return ret;
}

