#ifndef PARSE_CONFIG_HPP
# define PARSE_CONFIG_HPP

# include "server.hpp"
# include <vector>
# include <iostream>
# include <fstream>
# include <sstream>
# include <string>

class ParseConf {
    private :
    std::vector<server> l_server;

    public :
    ParseConf();
    ~ParseConf();

    std::vector<server> parse_server(std::string conf_path);
};

#endif
