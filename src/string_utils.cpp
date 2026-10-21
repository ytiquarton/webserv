#include <string>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

std::vector<char *> to_char_str_tab(std::vector<std::string> &input)
{
	std::vector<char *>	output;
	std::vector<std::string>::iterator index = input.begin();
	while (index != input.end())
	{
		output.push_back(const_cast<char *>((*index).c_str()));
		index++;
	}
	output.push_back(0);
	return (output);
}

std::string	to_lower(const std::string s)
{
	std::string output;
	for(std::size_t i=0; i < s.size(); i++)
	{
		output += std::tolower(static_cast<unsigned char>(s[i]));
	}
	return (output);
}

std::string	trim(const std::string s)
{
	std::string output;

	std::size_t start = s.find_first_not_of(" \t");
	std::size_t	end = s.find_last_not_of(" \t");

	if (start == std::string::npos)
		return ("");
	output = s.substr(start, end-start + 1);
	return (output);
}

std::string int_to_string(int nb)
{
	std::ostringstream ss;
	ss << nb;
	return ss.str();
}

bool	endsWith(const std::string& base, const std::string& substring)
{
	return (base.size() >= substring.size() && base.compare(base.size()-substring.size(), substring.size(), substring) == 0);
}
