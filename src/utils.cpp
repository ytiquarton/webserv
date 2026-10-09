#include "utils.hpp"
#include <sys/stat.h>
#include <string>
#include "http_message.hpp"

std::string appendRoute(Route target_route, std::string path)
{
    std::string root = target_route.getRoot();
	std::string output;
    if (root.empty()) 
        return path;

    if (!root.empty() && root[root.length() - 1] == '/' && !path.empty() && path[0] == '/') 
        root.erase(root.length() - 1);
    else if (!root.empty() && root[root.length() - 1] != '/' && !path.empty() && path[0] != '/')
		root += "/";
	output = root + path;
	if (output[0] == '/')
		output = output.substr(1);
    return output;
}

std::string return_path_routed(std::string path, ServerConfig& server)
{
	Route target_route = server.getOneRoute(path);

    std::pair<int, std::string> redir = target_route.getRedir();
    if (redir.first != 0)
		return redir.second;
	return (appendRoute(target_route, path.substr(target_route.getRoutePath().length())));
}


bool isDirectory(const std::string& path)
{
	struct stat info;
	return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}