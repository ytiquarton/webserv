#include "parse_config.hpp"

ParseConf::ParseConf(){}
ParseConf::~ParseConf(){}


//fonction pour enlever les espaces
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}
std::vector<server> ParseConf::parse_server(std::string conf_path){
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
        if (start_brace == std::string::npos) {
            std::cerr << "Error: Missing opening brace '{' for server block" << std::endl;
            break;
        }

        //Algorithme de comptage d'accolades pour gérer les blocs location {} imbriqués
        int brace_count = 1;
        size_t end_brace = start_brace + 1;

        while (end_brace < conf_str.length() && brace_count > 0) {
            if (conf_str[end_brace] == '{') brace_count++;
            else if (conf_str[end_brace] == '}') brace_count--;
            end_brace++;
        }

        if (brace_count != 0) {
            std::cerr << "Error: Unmatched closing brace '}' in server block" << std::endl;
            break;
        }
        //code du dessus pour verifier les accolades et les accolades imbriquées dans serv

        //extraction contenu server
        std::string block = conf_str.substr(start_brace + 1, end_brace - start_brace - 1);
        //parsing contenu
        server serv_temp;
        std::stringstream block_stream(block);
        std::string line;
        while(std::getline(block_stream, line)) {
            line = trim(line);

            if (line.empty() || line[0] == '#')
                continue;

            size_t equal_pos = line.find("=");
            size_t semi_pos = line.find(";");
            
            if (equal_pos != std::string::npos && semi_pos != std::string::npos && semi_pos > equal_pos) {
                std::string key = trim(line.substr(0, equal_pos));
                std::string val = trim(line.substr(equal_pos + 1, semi_pos - equal_pos - 1));

                //parse port
                if (key == "port" || key == "listen") {
                        int port_val = std::atoi(val.c_str());
                        if (port_val > 0 && port_val <= 65535) {
                            serv_temp.setPort(port_val);
                        } else {
                            std::cerr << "Error: Invalid port number: " << val << std::endl;
                        }
                }
            
            // Extraction du serv_name
                else if (key == "serv_name") {
                        serv_temp.setServerName(val);
                }

                //parse error page
                else if (key == "error_page") {
                    std::stringstream ss(val);
                    std::vector<int> codes;
                    std::string token;
                    std::string page_path;

                    while (ss >> token) {
                        if (std::isdigit(token[0])) {
                            codes.push_back(std::atoi(token.c_str()));
                        } else {
                            page_path = token;
                        }
                    }

                    for (size_t i = 0; i < codes.size(); ++i) {
                        serv_temp.setErrorPage(codes[i], page_path);
                    }
                }
                else if (key == "client_max_body_size") {
                    size_t unit_pos = val.find_first_not_of("0123456789");
                    std::string num_str;
                    char unit = '\0';

                    if (unit_pos == std::string::npos) {
                        num_str = val;
                    } else if (unit_pos == val.length() - 1) {
                        num_str = val.substr(0, unit_pos);
                        unit = val[unit_pos];
                    } else {
                        std::cerr << "Error client_max_body_size" << val << std::endl;
                        continue;
                    }

                    long long num = std::atoll(num_str.c_str());
                    long long bytes = num;

                    if (unit == 'K' || unit == 'k') bytes = num * 1024;
                    else if (unit == 'M' || unit == 'm') bytes = num * 1024 * 1024;
                    else if (unit == 'G' || unit == 'g') bytes = num * 1024 * 1024 * 1024;
                    else if (unit != '\0') {
                        std::cerr << "Error invalid unit '" << std::endl;
                        continue;
                    }

                    if (bytes < 0 || bytes > 2147483648LL) { // Max 2 GB
                        std::cerr << "Error must be within 0 and 2GB" << std::endl;
                    } else {
                        serv_temp.setClientMaxBodySize(static_cast<size_t>(bytes));
                    }
            }

        }
    }
    
}
