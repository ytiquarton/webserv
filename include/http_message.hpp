#pragma once
#include <string>
#include <map>

class http_message
{
	public:
	enum Methods
	{
		GET,
		POST,
		DELETE
	};

	http_message();

	bool	incomplete;
	std::string	start_line;
	std::map<std::string, std::string> header;
	std::string	message_body;
	static const	std::map<std::string, Methods> methods_map;
	static http_message parse_message(std::string message);

	Methods method;
	std::string	url;
	std::string version;

	std::string	left_over;
};
