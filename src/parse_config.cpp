#include "parse_config.hpp"

ParseConf::ParseConf(){}
ParseConf::~ParseConf(){}

std::vector<server> ParseConf::parse_server(std::string conf_path){
    int pos_server;
    std::vector<server> lst_serv;
    std::ifstream conf_file(conf_path);

    if (!conf_file.is_open()) {
        std::cerr << "Couldn't open config file" << std::endl;
        return lst_serv; //retourner une erreur
    }
    std::stringstream buffer;
    buffer << conf_file.rdbuf();

    conf_file.close();
    std::string conf_str = buffer.str();

    size_t pos = 0;

    while ((pos = conf_str.find("server", pos)) != std::string::npos) {
        size_t start_brace = conf_str.find("{", pos);
        size_t end_brace = conf_str.find("}", start_brace);

        if (start_brace == std::string::npos || end_brace == std::string::npos) {
            std::cerr << "Missing brace" << std::endl;
            break;
        }

        //extraction contenu server
        std::string block = conf_str.substr(start_brace + 1, end_brace - start_brace - 1);

        //parsing contenu
        server serv_temp;
        std::stringstream block_stream(block);
        std::string line;

        while(std::getline(block_stream, line)) {
            if (line.find("port") != std::string::npos) {
                size_t equal_pos = line.find("=");
                size_t semi_pos = line.find(";");
                if (equal_pos != std::string::npos && semi_pos != std::string::npos) {
                    std::string val = line.substr(equal_pos + 1, semi_pos - equal_pos - 1);
                    serv_temp.setPort(std::atoi(val.c_str()));
                }
            }
            // Extraction du serv_name
            else if (line.find("serv_name") != std::string::npos) {
                size_t equal_pos = line.find("=");
                size_t semi_pos = line.find(";");
                if (equal_pos != std::string::npos && semi_pos != std::string::npos) {
                    std::string val = line.substr(equal_pos + 1, semi_pos - equal_pos - 1);
                    
                    // Nettoyage des espaces éventuels au début ou à la fin
                    size_t first = val.find_first_not_of(" \t");
                    size_t last = val.find_last_not_of(" \t");
                    if (first != std::string::npos && last != std::string::npos) {
                        val = val.substr(first, (last - first + 1));
                    }
                    
                    serv_temp.setServerName(val);
                }
            }
        }
    }    
    
}
