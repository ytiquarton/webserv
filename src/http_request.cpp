#include <map>
#include "http_request.hpp"
#include <string>
#include <iostream>
#include <stdexcept>
#include <utility>
#include "string_utils.hpp"

http_request::http_request(): incomplete(true)
{

}

std::map<std::string, http_request::Methods>	make_methods_map()
{
	std::map<std::string, http_request::Methods>	output;

	output["GET"] = http_request::GET;
	output["POST"] = http_request::POST;
	output["DELETE"] = http_request::DELETE;
	return (output);
}

const std::map<std::string, http_request::Methods>
    http_request::methods_map = make_methods_map();




bool	is_request(http_request &message)
{
	if (http_request::methods_map.find(message.start_line.substr(0, message.start_line.find(" "))) != http_request::methods_map.end())
	{
		std::string::size_type first = message.start_line.find(" ");
		std::string::size_type second = message.start_line.find(" ", first + 1);
		std::string::size_type third = message.start_line.find(" ", second + 1);

		if (first == std::string::npos || second == std::string::npos || third != std::string::npos)
			throw std::runtime_error("Invalid request!");
		
		message.method = http_request::methods_map.find(message.start_line.substr(0, first))->second;
		message.url = message.start_line.substr(first + 1, second - first - 1);
		message.version = message.start_line.substr(second + 1);
		return (true);
	}
	return false;
}

std::string get_next_line(std::string& content)
{
	std::string	output;

	if (content.find("\r\n") == std::string::npos)
		throw (std::runtime_error("No endline!"));
	output = content.substr(0, content.find("\r\n"));
	content = content.substr(content.find("\r\n") + 2, std::string::npos);
	return (output);
}

std::pair<std::string, std::string>	get_pair(std::string key_value_pair)
{
	std::pair<std::string, std::string>	output;

	if (key_value_pair.find(':') == std::string::npos)
		throw (std::runtime_error("Invalid key-value pair!"));
	output.first = to_lower(key_value_pair.substr(0, key_value_pair.find(':')));
	output.second = trim(key_value_pair.substr(key_value_pair.find(':') + 1, std::string::npos));
	return (output);
}

http_request http_request::parse_message(std::string message)
{
	http_request	output;
	std::string		key_value_pair;

	try{
		output.start_line = get_next_line(message);
		
		if (!is_request(output))
			throw(std::runtime_error("Received http message isn't a request!"));
		while (true)
		{
			key_value_pair = get_next_line(message);
			if (key_value_pair == "")
			{
				std::cout << "Header End! \n";
				break;
			}
			output.headers.insert(get_pair(key_value_pair));
		}
		
	}catch(const std::exception& e)
	{
		if (std::string(e.what()) == std::string("No endline!"))
			return (output);
		throw(std::runtime_error(std::string("Wrong message format: ") + e.what()));
	}
	if((output.headers.find("Context-Length") != output.headers.end() && output.headers["Context-Length"] != "0") || output.headers.find("Transfer-Encoding") != output.headers.end())
		throw(std::runtime_error("Request contains a body!"));
	output.incomplete = false;
	output.left_over = message;
	return (output);
}