#pragma once
#ifndef ORDERFACTORY_HPP
# define ORDERFACTORY_HPP

#include "OrderCache.h"
#include <iostream>

class OrderFactory{
public:
	OrderFactory(void);
	OrderFactory(uint32_t maxSecIds, uint32_t buyRatio, uint32_t minQty, uint32_t maxQty, uint32_t maxUsers, uint32_t maxCompanies);
	~OrderFactory(void);
	void setMaxSecIds(uint32_t maxSecIds);
	void setBuyRatio(uint32_t buyRatio);
	void setMinQty(uint32_t minQty);
	void setMaxQty(uint32_t maxQty);
	void setMaxUsers(uint32_t maxUsers);
	void setMaxCompanies(uint32_t maxCompanies);
	void setQtyResolution(uint8_t resolution);
	void setIdIndex(uint32_t index);
	Order singleOrderGenerator(void);
	void batchOrderGenerator(uint32_t ammount, OrderCache &cache);
	void printSettings(void) const;
	static void printOrder(const Order &order);
	// these could've been templates, but since time is short, they aren't
	static void printOrderContainer(std::vector<Order> &container);
	static void printOrderContainer(std::unordered_map<std::string, Order> &container);
	static void printOrderContainer(std::deque<Order> &container);
private:
	uint32_t _maxSecIds;
	uint32_t _buyRatio;
	uint32_t _minQty;
	uint32_t _maxQty;
	uint32_t _maxUsers;
	uint32_t _maxCompanies;
	uint8_t _qtyResolution; // defines trailing zeros in qty
	uint32_t _idIndex;
};


#endif // ORDERFACTORY_HPP