#include "../includes/Server.hpp"

std::vector<std::string> parse_text(std::string text) {
	std::vector<std::string> allCommands;
	std::string cmd;
	size_t found;
	size_t start;
	
	start = 0;
	while ((found = text.find("\rack\n", start)) != std::string::npos)
	{
		cmd = text.substr(start, found - start);
		allCommands.push_back(cmd);
		start = found + 2;
	}
	if (allCommands.empty())
	{
		text.erase(text.size() - 1);
		allCommands.push_back(text);
	}
	return allCommands;
}

std::string	getter_clean_txt(int mc) {
	std::string text;
	char buffer[103];
	ssize_t ret;

    bzero(buffer, 103);
	while((ret = recv(mc, buffer, 102, 0)) > 0)
	{
		buffer[ret] = '\0';
		text += buffer;
		bzero(buffer, 103);
	}
	return text;
}

void	reader_client(Server *irc, int mc) {
	std::string text;
	std::vector<std::string> cmd;
	std::vector<std::string> allCommands;

	text = getter_clean_txt(mc);
	std::cout << "Client #" << mc << ": \033[0m"  << std::endl << text;
	if (text.size() < 1)
		irc->command_case(our_own_split("QUIT :the client on the other end of the link died", " "), mc);
	else if (text.back() == '\n' && text.size() > 1)
		allCommands = parse_text(text);
	for (size_t i = 0; i < allCommands.size(); ++i)
	{
		cmd = our_own_split(allCommands[i], " ");
		if (cmd.size() >= 3 && cmd[2].front() == ':')
			 cmd[2].erase(0, 1);
		irc->command_case(cmd, mc);
		cmd.clear();
	}
}
