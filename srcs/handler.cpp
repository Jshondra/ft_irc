#include "../includes/Server.hpp"

std::string strjoin_cmd(std::vector<std::string> vector, const std::string& delim, int start, int end) {
	std::string	result;
	std::vector<std::string>::iterator ret;
	
	for (ret = vector.begin() + start; ret != vector.begin() + end; ++ret)
		result += *ret + delim;
    result.erase(result.size() - 1);
	return result;
}

std::vector<std::string> our_own_split(const std::string& str, const std::string& sep)
{
	std::vector<std::string> allCommands;
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
			allCommands.push_back(found);
		}
		start = end + sep.length();
		end = str.find(sep, start);
	}
	if (start != 0)
	{
		found = str.substr(start, end);
		if (found.length() > 0)
			allCommands.push_back(found);
	}
	else
		allCommands.push_back(str);
	return allCommands;
}

void	Server::away_cmd(std::vector<std::string> cmd, int mc) {
    std::string message;

    if (cmd.size() == 1)
        message = "";
    else
        message = strjoin_cmd(cmd, " ", 1, cmd.size());
    if (message.front() == ':')
		message.erase(0,1);
    this->file_descriptors[mc].setaway_cmd(message);
}