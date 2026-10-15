#include "utils.hpp"
#include <sys/stat.h>
#include <string>
#include "http_request.hpp"
#include "parse_config.hpp"


std::string get_http_nb_phrase(int code)
{
	switch (code) {
		case 200:	return "OK";
		case 201:	return "Created";
		case 204:	return "No Content";
		case 301:	return "Moved Permanently";
		case 302:	return "Found";
		case 400:	return "Bad Request";
		case 403:	return "Forbidden";
		case 404:	return "Not Found";
		case 405:	return "Method Not Allowed";
		case 408:	return "Request Timeout";
		case 411:	return "Length Required";
		case 413:	return "Content Too Large";
		case 414:	return "URI Too Long";
		case 500:	return "Internal Server Error";
		case 501:	return "Not Implemented";
		case 502:	return "Bad Gateway";
		case 504:	return "Gateway Timeout";
		case 505:	return "HTTP Version Not Supported";
		default:	return "Unknown";
	}
}

std::string read_file(std::string path)
{
	std::ifstream file(path.c_str());
	if (!file)
		throw (http_request::http_error("File not found!", 404));

	std::ostringstream ss;
	ss << file.rdbuf();
	return (ss.str());
}

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

	return (appendRoute(target_route, path.substr(target_route.getRoutePath().length())));
}


bool isDirectory(const std::string& path)
{
	struct stat info;
	return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

bool existDir(const std::string& path) {
    struct stat info;

    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}
