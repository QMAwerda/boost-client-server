#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <iostream>
#include <system_error>
#include <thread>
#include <vector>

int main() {
  boost::system::error_code ec; // asio::error_code

  // Create a "context" - the platform specific interface
  boost::asio::io_context context;

  boost::asio::ip::tcp::endpoint endpoint(
      boost::asio::ip::make_address("51.38.81.49", ec), 80);
  // 93.184.216.34
  // 51.38.81.49

  // The context will deliver the implementation
  boost::asio::ip::tcp::socket socket(context);

  // Tell socket to try and connect
  socket.connect(endpoint, ec);

  if (!ec) {
    std::cout << "Connected!\n";
  } else {
    std::cout << "Failed to connect to adress: " << ec.message() << "\n";
  }

  if (socket.is_open()) {

    std::string sRequest = "GET /index.html HTTP/1.1\r\n"
                           "Host: example.com\r\n"
                           "Connection: close\r\n\r\n";

    // Send data to the address
    // send and get data work in asio with asio::buffer (it's a container of
    // bytes)
    socket.write_some(boost::asio::buffer(sRequest.data(), sRequest.size()),
                      ec);

    socket.wait(socket.wait_read); // waiting answer from server

    size_t bytes = socket.available(); // show if we have avalable bytes to read
    std::cout << "Bytes Available: " << bytes << "\n";

    if (bytes > 0) {
      std::vector<char> vBuffer(bytes);
      // asio work below in syncro mode
      socket.read_some(boost::asio::buffer(vBuffer.data(), vBuffer.size()), ec);

      for (auto c : vBuffer) {
        std::cout << c;
      }
    }
  }
}