#pragma once
#include <string>
#include <map>

class http_message
{
	public:
	std::string	header;
	std::map<std::string, std::string> content;
	std::string content_str;
};