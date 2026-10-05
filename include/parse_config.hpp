#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <exception>

class Route;
class Server;

class Route {
    private :
        std::string _route_path;
        std::pair<int, std::string> _redir; // redirection 
        std::string _allow_methods;
        std::string _root;
        bool _autoindex; //
        std::string _index;
        std::string _upload_store;
        std::string _cgi_pass;
        std::string _cgi_dir;

    public :
        Route() : _route_path(""), _redir(0, ""), _allow_methods(""), _root(""), _autoindex(false), _index(""){}

        // _route_path
        void setRoutePath(const std::string& path) {this->_route_path = path;}
        std::string getRoutePath() const {return (this->_route_path);}

        // _redir

        void setRedir(int code, const std::string& path) {
            this->_redir.first = code;
            this->_redir.second = path;
        }
        std::pair<int, std::string> getRedir() const {return this->_redir;}
        std::string getRedirPath() const {return this->_redir.second;}

        // _allow_methods
        void setAllowMethods(const std::string& a_m) {this->_allow_methods = a_m;}
        std::string getAllowMethods() const {return (this->_allow_methods);}

        // _root

        void setRoot(const std::string& r) {this->_root = r;}
        std::string getRoot() const {return(this->_root);}

        // _autoindex

        void setAutoindex(bool a_i) {this->_autoindex = a_i;}
        bool getAutoindex() const {return (this->_autoindex);}

        // _index

        void setIndex(const std::string& ix) {this->_index = ix;}
        std::string getIndex() const {return (this->_index);}

        // _upload_store

        void setUploadStore(const std::string& u_s) {this->_upload_store = u_s;}
        std::string getUploadStore() const {return (this->_upload_store);}

        // _cgi_pass

        void setCgiPass(const std::string& c_p) {this->_cgi_pass = c_p;}
        std::string getCgiPass() const {return (this->_cgi_pass);}

        // _cgi_dir

        void setCgiDir(const std::string& c_d) {this->_cgi_dir = c_d;}
        std::string getCgiDir() const {return (this->_cgi_dir);}
};



class ServerConfig {
    private :
        int         _port;
        std::string _server_name;
        std::map<int, std::string> _error_pages;
        size_t _max_size;
        std::map<std::string, Route> _lst_routes;

    public :
        ServerConfig() : _port(8080), _server_name(""), _max_size(1048576) {}

        void setPort(int p) {this->_port = p;}
        void setServerName(const std::string& name) {this->_server_name = name;}
        int getPort() const {return _port;}
        std::string getServerName() const {return _server_name;}

        // error pages
        const std::map<int, std::string> & getErrorPages() const {
            return this->_error_pages;
        }

        std::string getErrorPagePath(int code) const {
            std::map<int, std::string>::const_iterator it = this->_error_pages.find(code);
            if (it != this->_error_pages.end()) {
                return it->second;
            }
            return "";
        }

        void setErrorPage(int code, const std::string& path) {
            this->_error_pages[code] = path;
        }
        //client max body

        size_t getClientMaxBodySize() const {
            return this->_max_size;
        }
        void setClientMaxBodySize(size_t size) {
            this->_max_size = size;
        }

        // lst routes

        const std::map<std::string, Route>& getRoutes() const {
            return this->_lst_routes;
        }

        Route getOneRoute(std::string path) const {
            std::map<std::string, Route>::const_iterator it = this->_lst_routes.find(path);
            if (it != this->_lst_routes.end())
                return it->second;
            Route error_route;
            return error_route;
        }

        void setRoute(const std::string& path, const Route& c_route){
            this->_lst_routes[path] = c_route;
        }

};

Route parse_route(std::string route, std::string block_route);
Server parse_server(std::string conf_path);
std::string return_path_routed(std::string path);



#endif