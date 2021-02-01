#include <boost/asio.hpp>
#include <array>

class Client {
public:
    Client(const std::string& ip_address_host,
           const std::string& server_port,
           unsigned short local_port = 0);
    ~Client();
    void send(const std::array<char, 1>& b);
    size_t receive(std::array<char, 128>& b);
private:
    boost::asio::io_context io_context;
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint server_endpoint;
    boost::asio::ip::udp::endpoint remote_endpoint;
};