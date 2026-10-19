#include <map>
#include "http_request.hpp"
#include <string>
#include <iostream>
#include <stdexcept>
#include <utility>
#include "string_utils.hpp"
#include <cstdlib>


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

std::map<http_request::Methods, std::string>	make_r_methods_map()
{
	std::map<http_request::Methods, std::string>	output;

	output[http_request::GET] = "GET";
	output[http_request::POST] = "POST";
	output[http_request::DELETE] = "DELETE";
	return (output);
}

const	std::map<http_request::Methods, std::string> 
	http_request::r_methods_map = make_r_methods_map();



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
		if (message.url.find("?") != std::string::npos)
		{
			message.query = message.url.substr(message.url.find("?") + 1);
			message.url = message.url.substr(0, message.url.find("?"));
		}
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

void	parse_headers(http_request &message, std::string &str_message)
{
	std::string		key_value_pair;
	while (true)
	{
		key_value_pair = get_next_line(str_message);
		if (key_value_pair == "")
		{
			std::cout << "Header End! \n";
			break;
		}
		message.headers.insert(get_pair(key_value_pair));
	}
}

void	parse_body(http_request &message, std::string &str_message)
{
		bool	hasContentLength = message.headers.find("content-length") != message.headers.end();
		bool	hasChunkTransfer = message.headers.find("transfer-encoding") != message.headers.end() && message.headers["transfer-encoding"].find("chunked") != std::string::npos;
		if (hasChunkTransfer && hasContentLength)
			throw(http_request::http_error("Invalid headers!", 400));
		if((hasContentLength && message.headers["content-length"] != "0"))
		{
			int len = std::atoi(message.headers["content-length"].c_str());
			if (str_message.size()<len)
				throw(std::runtime_error("Incomplete!"));
			message.message_body = str_message.substr(0, len);
			message.left_over = str_message.substr(len);
		}
		else if(hasChunkTransfer)
		{
			int	len;
			while(true)
			{
				len = std::atoi(get_next_line(str_message).c_str());
				if (str_message.size()<len)
					throw(std::runtime_error("Incomplete!"));
				if (len == 0)
					break;
				message.message_body.append(str_message.substr(0, len));
				str_message = str_message.substr(len);
				if (str_message.substr(0, 2) != "\r\n")
					throw(std::runtime_error("Incomplete!"));
					
			}
		}
}

http_request http_request::parse_message(std::string message)
{
	http_request	output;

	try{
		output.start_line = get_next_line(message);
		
		if (!is_request(output))
			throw(std::runtime_error("Received http message isn't a request!"));
		parse_headers(output, message);
		parse_body(output, message);

		

		
	}catch(const std::exception& e)
	{
		if (std::string(e.what()) == std::string("No endline!") || 
				std::string(e.what()) == std::string("Incomplete!") )
		{
			return (output);
		}
		throw(std::runtime_error(std::string("Wrong message format: ") + e.what()));
	}
	output.incomplete = false;
	output.left_over = message;
	return (output);
}