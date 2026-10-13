#pragma once
#include <string>
#include <map>
#include <stdexcept>

class http_request
{
	public:
	enum Methods
	{
		GET,
		POST,
		DELETE
	};

	http_request();

	bool	incomplete;
	std::string	start_line;
	std::map<std::string, std::string> headers;
	std::string	message_body;
	static const	std::map<std::string, Methods> methods_map;
	static http_request parse_message(std::string message);

	Methods method;
	std::string	url;
	std::string version;
	std::string	left_over;

	class http_error : public std::runtime_error
	{
		private:
			const int nb;
		public:
			http_error(const std::string& msg, int _nb): runtime_error(msg), nb(_nb)
			{

			}
			int get_nb() const
			{
				return nb;
			}
	};
};
