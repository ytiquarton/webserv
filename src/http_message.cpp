#include <map>
#include "http_message.hpp"
#include <string>
#include <iostream>
#include <stdexcept>
#include <utility>
#include "string_utils.hpp"

http_message::http_message(): incomplete(true)
{

}

std::map<std::string, http_message::Methods>	make_methods_map()
{
	std::map<std::string, http_message::Methods>	output;

	output["GET"] = http_message::GET;
	output["POST"] = http_message::POST;
	output["DELETE"] = http_message::DELETE;
	return (output);
}

const std::map<std::string, http_message::Methods>
    http_message::methods_map = make_methods_map();




bool	is_request(std::string start_line)
{
	return http_message::methods_map.find(start_line.substr(0, start_line.find(" "))) != http_message::methods_map.end();
}

std::string get_next_line(std::string& content)
{
	std::string	output;

	std::cout << "Getting next line... " << std::endl;
	if (content.find("\r\n") == std::string::npos)
		throw (std::runtime_error("No endline!"));
	output = content.substr(0, content.find("\r\n"));
	content = content.substr(content.find("\r\n") + 2, std::string::npos);

	std::cout << "Found line: \n" << output << std::endl;
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

http_message http_message::parse_message(std::string message)
{
	http_message	output;
	std::string		key_value_pair;

	try{
		output.start_line = get_next_line(message);
		
		if (!is_request(output.start_line))
			throw(std::runtime_error("Received http message isn't a request!"));
		while (true)
		{
			key_value_pair = get_next_line(message);
			if (key_value_pair == "")
			{
				std::cout << "Header End! \n";
				break;
			}
			output.header.insert(get_pair(key_value_pair));
		}
		
	}catch(const std::exception& e)
	{
		if (e.what() == "No endline!")
			return (output);
		throw(std::runtime_error(std::string("Wrong message format: ") + e.what()));
	}
	if((output.header.find("Context-Length") != output.header.end() && output.header["Context-Length"] != "0") || output.header.find("Transfer-Encoding") != output.header.end())
		throw(std::runtime_error("Request contains a body!"));
	output.incomplete = false;
	output.left_over = message;
	return (output);
}