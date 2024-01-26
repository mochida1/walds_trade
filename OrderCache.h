#pragma once
#ifndef ORDERCACHE_H
# define ORDERCACHE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <algorithm>
#include <execution>
#include <cassert>

class Order
{

 public:

  // do not alter signature of this constructor
  Order(
      const std::string& ordId,
      const std::string& secId,
      const std::string& side,
      const unsigned int qty,
      const std::string& user,
      const std::string& company)
      : m_orderId(ordId),
        m_securityId(secId),
        m_side(side),
        m_qty(qty),
        m_user(user),
        m_company(company) { }

  // do not alter these accessor methods
  std::string orderId() const    { return m_orderId; }
  std::string securityId() const { return m_securityId; }
  std::string side() const       { return m_side; }
  std::string user() const       { return m_user; }
  std::string company() const    { return m_company; }
  unsigned int qty() const       { return m_qty; }

 private:

  // use the below to hold the order data
  // do not remove the these member variables
  std::string m_orderId;     // unique order id
  std::string m_securityId;  // security identifier
  std::string m_side;        // side of the order, eg Buy or Sell
  unsigned int m_qty;        // qty for this order
  std::string m_user;        // user name who owns this order
  std::string m_company;     // company for user

};

class OrderToMatch : public Order
{
public:
	OrderToMatch(
        const std::string& ordId,
        const std::string& secId,
        const std::string& side,
        const unsigned int qty,
        const std::string& user,
        const std::string& company,
        unsigned int _qty,
		bool _isMatch)
        : Order(ordId, secId, side, qty, user, company),
          _qty(qty),
		  _isMatch(false) {}
	OrderToMatch(const Order& order)
        : Order(order), _qty(order.qty()) {this->_isMatch = false;}
	void setQty(unsigned int newQty) {this->_qty = newQty;}
	unsigned int getQty(void) {return this->_qty;}
	bool isMatch(void){return this->_isMatch;}
	void setIsMatch(void){this->_isMatch = true;}
private:
	unsigned int _qty;
	bool _isMatch;
};

class OrderCacheInterface
{

 public:

  // implement the 6 methods below, do not alter signatures

  // add order to the cache
  virtual void addOrder(Order order) = 0;

  // remove order with this unique order id from the cache
  virtual void cancelOrder(const std::string& orderId) = 0;

  // remove all orders in the cache for this user
  virtual void cancelOrdersForUser(const std::string& user) = 0;

  // remove all orders in the cache for this security with qty >= minQty
  virtual void cancelOrdersForSecIdWithMinimumQty(const std::string& securityId, unsigned int minQty) = 0;

  // return the total qty that can match for the security id
  virtual unsigned int getMatchingSizeForSecurity(const std::string& securityId) = 0;

  // return all orders in cache in a vector
  virtual std::vector<Order> getAllOrders() const = 0;
};

// Provide an implementation for the OrderCacheInterface interface class.
// Your implementation class should hold all relevant data structures you think
// are needed.
class OrderCache : public OrderCacheInterface
{

 public:
  //--------------do not touch these
  void addOrder(Order order) override;
  void cancelOrder(const std::string& orderId) override;
  void cancelOrdersForUser(const std::string& user) override;
  void cancelOrdersForSecIdWithMinimumQty(const std::string& securityId, unsigned int minQty) override;
  unsigned int getMatchingSizeForSecurity(const std::string& securityId) override;
  std::vector<Order> getAllOrders() const override;
  // you can touch whataver comes after-------------
  
  OrderCache(void);
  ~OrderCache();
  
  // remove after use
  void assertOrderNumbers(void);

 private:
 //unordered map that uses orderId as key;
  std::unordered_map<std::string, Order> _orders;
  mutable std::mutex _mutex;
  const std::string _buyLookUpTerm = "Buy";
  const std::string _sellLookUpTerm = "Sell";
  uint32_t _totalOrders;
  uint32_t _totalBuyOrders;
  uint32_t _totalSellOrders;
  
  };

#endif //ORDERCACHE_H