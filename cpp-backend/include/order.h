#ifndef ORDER_H
#define ORDER_H

#include <boost/serialization/access.hpp>

#include <chrono>
#include <cstdint>
#include <string>

enum class Side { Buy, Sell };



struct Order {
    uint64_t id;
    Side side;
    double price;
    uint32_t quantity;

    Order() = default;
    Order(uint64_t id, Side side, double price, uint32_t quantity)
        : id(id), side(side), price(price), quantity(quantity) {}

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int) {
        ar & id;
        ar & side;
        ar & price;
        ar & quantity;
    }
};


#endif
