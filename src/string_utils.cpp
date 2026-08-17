#include <string>
#include <cctype>

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