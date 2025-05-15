#include <algorithm>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/asio.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "orderbook.h"

using boost::asio::ip::tcp;

OrderBook orderBook;

void saveOrderBook(const OrderBook &book) {
  std::ofstream ofs("orderbook.dat", std::ios::binary);
  if (ofs) {
    boost::archive::binary_oarchive oa(ofs);
    oa << book;
  }
}

void loadOrderBook(OrderBook &book) {
  std::ifstream ifs("orderbook.dat", std::ios::binary);
  if (ifs) {
    boost::archive::binary_iarchive ia(ifs);
    ia >> book;
  }
}

class Session : public std::enable_shared_from_this<Session> {
  tcp::socket socket_;
  boost::asio::streambuf buffer_;

public:
  explicit Session(tcp::socket socket) : socket_(std::move(socket)) {}

  void start() { do_read(); }

private:
  void do_read() {
    auto self(shared_from_this());
    boost::asio::async_read_until(
        socket_, buffer_, '\n',
        [this, self](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            std::istream is(&buffer_);
            std::string line;
            std::getline(is, line);
            if (!line.empty() && line.back() == '\r') // remove CR for telnet
              line.pop_back();

            std::string response = process_command(line);
            do_write(response);
          }
          // connection closed or error: session ends
        });
  }

  std::string process_command(const std::string &cmdline) {
    std::istringstream iss(cmdline);
    std::string cmd;
    iss >> cmd;

    if (cmd == "ADD_ORDER") {
      uint64_t id;
      std::string sideStr;
      double price;
      uint32_t qty;
      if (!(iss >> id >> sideStr >> price >> qty))
        return "ERROR: Invalid ADD_ORDER format\n";

      Side side;
      if (sideStr == "Buy")
        side = Side::Buy;
      else if (sideStr == "Sell")
        side = Side::Sell;
      else
        return "ERROR: Side must be Buy or Sell\n";

      orderBook.addOrder(Order{id, side, price, qty});
      saveOrderBook(orderBook);
      return "OK: Order added\n";
    } else if (cmd == "EXECUTE_ORDER") {
      uint64_t id;
      if (!(iss >> id))
        return "ERROR: Invalid EXECUTE_ORDER format\n";

      if (orderBook.executeOrder(id)) {
        saveOrderBook(orderBook);
        return "OK: Order executed\n";
      } else {
        return "ERROR: Order not found\n";
      }
    } else if (cmd == "PRINT_BOOK") {
      return orderBook.printBook();
    } else {
      return "ERROR: Unknown command\n";
    }
  }

  void do_write(const std::string &msg) {
    auto self(shared_from_this());
    boost::asio::async_write(
        socket_, boost::asio::buffer(msg),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            do_read();
          }
        });
  }
};

class Server {
  boost::asio::io_context &io_context_;
  tcp::acceptor acceptor_;

public:
  Server(boost::asio::io_context &io_context, short port)
      : io_context_(io_context),
        acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    do_accept();
  }

private:
  void do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::make_shared<Session>(std::move(socket))->start();
          }
          do_accept();
        });
  }
};

int main() {
  try {
    loadOrderBook(orderBook);

    boost::asio::io_context io_context;
    Server server(io_context, 8080);

    std::cout << "TCP OrderBook server listening on port 8080\n";

    io_context.run();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

