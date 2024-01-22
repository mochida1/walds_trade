#include "OrderCache.h"

OrderCache::OrderCache() : m_mutex(), m_orderCacheManager(), m_ordersByUserManager(),
    m_qtyByCompanyBySecIdManager(), m_orderQtyByOrderIdBySecIdManager()
{
}

OrderCache::~OrderCache()
{
}

void OrderCache::addOrder(Order order)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_orderCacheManager.addOrder(order);
    m_ordersByUserManager.addOrder(order);
    m_orderQtyByOrderIdBySecIdManager.addOrder(order);

    try {
        m_qtyByCompanyBySecIdManager.addOrder(order);
    } catch (std::runtime_error e) {
        m_orderCacheManager.cancelOrder(order.orderId());
        m_ordersByUserManager.cancelOrder(order);
        m_orderQtyByOrderIdBySecIdManager.cancelOrder(order);
        throw e;
    }
}

void OrderCache::cancelOrder(const std::string& orderId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    cancelOrderFromAllBuffers(orderId);
}

void OrderCache::cancelOrdersForUser(const std::string& user)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (const auto& orderId : m_ordersByUserManager.getAllUserOrders(user)) {
        cancelOrderFromAllBuffers(orderId);
    }
}

void OrderCache::cancelOrdersForSecIdWithMinimumQty(const std::string& securityId, unsigned int minQty)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (const auto& orderId : m_orderQtyByOrderIdBySecIdManager.getOrdersForSecIdWithMinimumQty(securityId, minQty)) {
        cancelOrderFromAllBuffers(orderId);
    }
}

unsigned int OrderCache::getMatchingSizeForSecurity(const std::string& securityId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_qtyByCompanyBySecIdManager.getMatchingSizeForSecurity(securityId);
}

std::vector<Order> OrderCache::getAllOrders() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_orderCacheManager.getAllOrders();
}

void OrderCache::cancelOrderFromAllBuffers(const std::string& orderId)
{
    Order order = m_orderCacheManager.cancelOrder(orderId);
    m_ordersByUserManager.cancelOrder(order);
    m_orderQtyByOrderIdBySecIdManager.cancelOrder(order);
    m_qtyByCompanyBySecIdManager.cancelOrder(order);
}
