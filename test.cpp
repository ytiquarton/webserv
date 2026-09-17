#include "./include/parse_config.hpp"

ParseConf::ParseConf(){}
ParseConf::~ParseConf(){}

std::vector<server> ParseConf::parse_server(std::string conf_path){
    int pos_server;
    std::vector<server> test;
    std::ifstream conf_file(conf_path);

    if (!conf_file.is_open()) {
        std::cerr << "Couldn't open config file" << std::endl;
        //retourner une erreur
    }
    std::stringstream buffer;
    buffer << conf_file.rdbuf();

    conf_file.close();

    std::string conf_str = buffer.str();
    std::cout << conf_str << std::endl;
    return (test)
}


int main(void)
{
    ParseConf test;
    std::vector<server> serv;

    serv = test.parse_server("./config.conf");

    return(0);
}