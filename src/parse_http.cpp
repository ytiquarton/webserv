#include <string>
#include "http_message.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>
#include <map>

std::string get_next_line(std::string& content)
{
	std::string	output;

	if (content.find("\n\r") == std::string::npos)
		throw (std::runtime_error("Invalid content!"));
	output = content.substr(0, content.find("\r\n"));
	content = content.substr(content.find("\r\n") + 2, std::string::npos);

	return (output);
}

std::pair<std::string, std::string>	get_pair(std::string key_value_pair)
{
	std::pair<std::string, std::string>	output;

	if (key_value_pair.find(':') == std::string::npos)
		throw (std::runtime_error("Invalid key-value pair!"));
	output.first = key_value_pair.substr(0, key_value_pair.find(':'));
	output.second = key_value_pair.substr(key_value_pair.find(':') + 1, std::string::npos);
}

http_message parse_message(std::string message)
{
	http_message	output;
	int				index;
	std::string		key_value_pair;

	try{
		output.header = get_next_line(message);
		index = 0;
		while (true)
		{
			key_value_pair = get_next_line(message);
			if (key_value_pair == "")
			{
				std::cout << "Header End! \n";
				break;
			}
			output.content.insert(get_pair(key_value_pair));
		}
		
	}catch(...)
	{
		throw(std::runtime_error("Wrong message format!"));
	}
	return (output);
}