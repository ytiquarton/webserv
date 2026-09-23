#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <cstddef>

class server {
    private :
        int         port;
        std::string server_name;
        std::map<int, std::string> error_pages;
        size_t max_size;

    public :
        server() : port(0), server_name(""), max_size(1048576) {}

        void setPort(int p) {this->port = p;}
        void setServerName(const std::string& name) {this->server_name = name;}
        int getPort() const {return port;}
        std::string getServerName() const {return server_name;}

        // error pages
        const std::map<int, std::string> & getErrorPages() const {
            return this->error_pages;
        }
        void setErrorPage(int code, const std::string& path) {
            this->error_pages[code] = path;
        }
        //client max body

        size_t getClientMaxBodySize() const {
            return this->max_size;
        }
        void setClientMaxBodySize(size_t size) {
            this->max_size = size;
        }

};


#endif