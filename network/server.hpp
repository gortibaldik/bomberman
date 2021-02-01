#include <boost/asio.hpp>

class Server {
public:
    Server(const std::string& server_port);
    ~Server();
    void loop();
private:
    boost::asio::io_context io_context;
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint remote_endpoint;
};