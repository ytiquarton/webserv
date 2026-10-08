#include "utils.hpp"

std::string return_path_routed(std::string path, ServerConfig& server){
    const std::map<std::string, Route>& routes = server.getRoutes();
    std::string best_match = "";
    Route target_route;

    for (std::map<std::string, Route>::const_iterator it = routes.begin(); it != routes.end(); it++) {
        std::string route_key = it->first;

        if (path.find(route_key) == 0) {
            if (path.length() == route_key.length() || path[route_key.length()] == '/' || route_key == "/") {
                if (route_key.length() > best_match.length()) {
                    best_match = route_key;
                    target_route = it->second;
                }
            }
        }
    }
    if (best_match.empty())
        return path;

    std::pair<int, std::string> redir = target_route.getRedir();
    if (redir.first != 0)
        return redir.second;
    
    std::string root = target_route.getRoot();
    if (root.empty()) 
        return path;
    
    std::string sub_path = path.substr(best_match.length());
    if (!root.empty() && root[root.length() - 1] == '/' && !sub_path.empty() && sub_path[0] == '/') 
        root.erase(root.length() - 1);
    else if (!root.empty() && root[root.length() - 1] != '/' && !sub_path.empty() && sub_path[0] != '/')
        root += "/";
    return root + sub_path;

}

