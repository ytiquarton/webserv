#pragma once
#include <string>
#include <map>

class http_message
{
	public:
	std::string	start_line;
	std::map<std::string, std::string> content;
	std::string content_str;
};

class http_request : public http_message
{
	public:
	std::string	header;
	std::map<std::string, std::string> content;
	std::string content_str;
};
class  http_response : public http_message
{
	public:

};