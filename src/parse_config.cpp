#include "../include/parse_config.hpp"


//fonction pour enlever les espaces premier et derniers espaces
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}


Route parse_route(std::string route, std::string block_route){
    std::string line;
    std::stringstream route_stream(block_route);
    Route route_ret;
    route_ret.setRoutePath(route);
    while (std::getline(route_stream, line)){
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;
        size_t equal_pos = line.find("=");
        size_t semi_pos = line.find(";");
        if (equal_pos != std::string::npos && semi_pos != std::string::npos && semi_pos > equal_pos) {
            std::string key = trim(line.substr(0, equal_pos));
            std::string val = trim(line.substr(equal_pos + 1, semi_pos - equal_pos - 1));
        
            /* parse _std::string _route_path;
            std::string _allow_methods;
            std::string _root;
            bool _autoindex; //
            std::string _index;
            std::string _upload_store;
            std::string _cgi_pass;
            std::string _cgi_dir;
            */
            if (key == "allow_methods") {
                route_ret.setAllowMethods(val);
            } else if (key == "root") {
                route_ret.setRoot(val);
            } else if (key == "autoindex") {
                if (val == "on" || val == "true" || val == "1")
                    route_ret.setAutoindex(true);
            } else if (key == "index") {
                route_ret.setIndex(val);
            } else if (key == "upload_store") {
                route_ret.setUploadStore(val);
            } else if (key == "cgi_pass") {
                if (val.compare(0, 3, ".py") != 0)
                    throw std::runtime_error("Wrong file format for cgi_pass");
                route_ret.setCgiPass(val);
            } else if (key == "cgi_dir") {
                
                route_ret.setCgiDir(val);
            }
        }
    }
    return route_ret;
}



Server parse_server(std::string conf_path){
    Server serv;
    std::ifstream conf_file(conf_path);

    if (!conf_file.is_open()) {
        std::cerr << "Couldn't open config file" << std::endl;
        return serv; //retourner un lst_serv vide en cas d'erreur
    }

    std::stringstream buffer;
    buffer << conf_file.rdbuf();
    conf_file.close();

    std::string conf_str = buffer.str();

        std::stringstream block_stream(conf_str);
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
            if (key == "port") {
                    int port_val = std::atoi(val.c_str());
                    if (port_val > 0 && port_val <= 65535) {
                        serv.setPort(port_val);
                    } else {
                        std::cerr << "Error: Invalid port number: " << val << std::endl;
                    }
            }
            // Extraction du serv_name
            else if (key == "serv_name") {
                    serv.setServerName(val);
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
                    serv.setErrorPage(codes[i], page_path);
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
                    serv.setClientMaxBodySize(static_cast<size_t>(bytes));
                }
            }
            else if (key == "location") 
            {
                std::string block_route = "";
                int brace_count = 0;

                std::string inner_line;

                while (std::getline(block_stream, inner_line)) {
                    size_t open_b = inner_line.find("{");
                    size_t close_b = inner_line.find("}");
                    if (open_b != std::string::npos)
                        brace_count++;
                    if (close_b != std::string::npos) {
                        if (brace_count > 0)
                            brace_count--;
                        if (brace_count == 0)
                            break;
                    }
                    if (brace_count > 0 && open_b == std::string::npos) {
                        block_route += inner_line + "\n";
                    }
                }
                Route r = parse_route(val, block_route);
                serv.setRoute(val, r);
            }

        }
    }
    return serv;

}

