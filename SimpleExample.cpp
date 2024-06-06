#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <iostream>
#include <system_error>
#include <thread>
#include <vector>

std::vector<char> vBuffer(
    20 * 1024); // large buffer for data, cause we don't know how much we need.
// grab some data and return it into socket:

void GrabSomeData(boost::asio::ip::tcp::socket &socket) {
  socket.async_read_some(boost::asio::buffer(vBuffer.data(), vBuffer.size()),
                         [&](std::error_code ec, std::size_t length) {
                           // length = how many bytes was read
                           if (!ec) {
                             std::cout << "\n\nRead " << length << " bytes\n\n";

                             for (int i = 0; i < length; i++) {
                               std::cout << vBuffer[i];
                             }

                             GrabSomeData(socket);
                           }
                         });
  // how it works:
  // it's not a recoursive nightmare; thanks to asio it will work in background.
  // It will take such many data as it can than it will call that func again and
  // again, until all data will be seen;  After that, it will waiting new data
  // in background. So, we don't need to take data manually, cause asio make it
  // in the background for us.
}

int main() {
  boost::system::error_code ec; // asio::error_code

  // Create a "context" - the platform specific interface
  boost::asio::io_context context;

  // Give some fake tasks to asio so the context doesn't finish
  // It will work in the bacground, so asio can't stop
  boost::asio::io_context::work idleWork(context);

  // We shoud start context in his own thread
  std::thread thrContext = std::thread([&]() { context.run(); });

  // Get the address of somewhere we wish to connect to
  // asio can connect to many types input/output, not only networks, but it's
  // always endpoint:
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

    GrabSomeData(socket);

    std::string sRequest = "GET /index.html HTTP/1.1\r\n"
                           "Host: example.com\r\n"
                           "Connection: close\r\n\r\n";

    // Send data to the address
    // send and get data work in asio with asio::buffer (it's a container of
    // bytes)
    socket.write_some(boost::asio::buffer(sRequest.data(), sRequest.size()),
                      ec);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(2000ms); // 2 seconds
  }
}