#include "../includes/Server.hpp"

std::string strjoin(std::vector<std::string> vector, const std::string& delim, int start, int end) {
	std::string	result;
	std::vector<std::string>::iterator ret;
	
	for (ret = vector.begin() + start; ret != vector.begin() + end; ++ret)
		result += *ret + delim;
    result.erase(result.size() - 1);
	return result;
}

std::vector<std::string> split(const std::string& str, const std::string& sep)
{
	std::vector<std::string> commands;
	std::string found;
	std::size_t start;
	std::size_t end;

	start = 0;
	end = str.find(sep);
	while (end != std::string::npos)
	{
		if (start != end)
		{
			found = str.substr(start, end - start);
			commands.push_back(found);
		}
		start = end + sep.length();
		end = str.find(sep, start);
	}
	if (start != 0)
	{
		found = str.substr(start, end);
		if (found.length() > 0)
			commands.push_back(found);
	}
	else
		commands.push_back(str);
	return commands;
}
