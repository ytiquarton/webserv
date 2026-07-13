#include <string>
#include "http_message.hpp"
#include <iostream>

http_message parse_message(std::string message)
{
	http_message	output;
	std::string		content_str;
	try{
		output.header = message.substr(0, message.find("\n"));
		output.content_str = message.substr(message.find("\n"), std::string::npos);
		std::cout<< "Header: " << output.header << "\n Content: " << content_str;
	}catch(...)
	{
		throw("Wrong message format!");
	}
	return (output);
}