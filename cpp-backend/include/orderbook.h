#pragma once
#include "order.h"
#include <vector>
#include <algorithm>
#include <string>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/access.hpp>

class OrderBook {
public:
    std::vector<Order> orders;

    void addOrder(const Order& order) {
        orders.push_back(order);
    }

    bool executeOrder(uint64_t orderId) {
        auto it = std::remove_if(orders.begin(), orders.end(),
                                 [orderId](const Order& o) { return o.id == orderId; });
        if (it != orders.end()) {
            orders.erase(it, orders.end());
            return true;
        }
        return false;
    }

    std::string printBook() const {
        std::string result;
        for (const auto& order : orders) {
            result += "ID: " + std::to_string(order.id) +
                      ", Side: " + (order.side == Side::Buy ? "Buy" : "Sell") +
                      ", Price: " + std::to_string(order.price) +
                      ", Qty: " + std::to_string(order.quantity) + "\n";
        }
        return result;
    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int) {
        ar & orders;
    }
};
