#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>

class server {
    private :
        int         port;
        std::string server_name;
    public :
        server() : port(0), server_name("") {}

        void setPort(int p) {port = p;}
        void setServerName(const std::string& name) {server_name = name;}
        int getPort() const {return port;}
        std::string getServerName() const {return server_name;}
};


#endif