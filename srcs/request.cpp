#include "../includes/Server.hpp"

std::vector<std::string> parse_text(std::string text) {
	std::vector<std::string> commands;
	std::string cmd;
	size_t found;
	size_t start;
	
	start = 0;
	while ((found = text.find("\r\n", start)) != std::string::npos)
	{
		cmd = text.substr(start, found - start);
		commands.push_back(cmd);
		start = found + 2;
	}
	if (commands.empty())
	{
		text.erase(text.size() - 1);
		commands.push_back(text);
	}
	return commands;
}

std::string	get_text(int cs) {
	std::string text;
	char buffer[103];
	ssize_t ret;

    bzero(buffer, 103);
	while((ret = recv(cs, buffer, 102, 0)) > 0)
	{
		buffer[ret] = '\0';
		text += buffer;
		bzero(buffer, 103);
	}
	return text;
}

void	client_read(Server *irc, int cs) {
	std::string text;
	std::vector<std::string> cmd;
	std::vector<std::string> commands;

	text = get_text(cs);
	std::cout << "Client #" << cs << ": " << std::endl << text;
	if (text.size() < 1)
		irc->choose_cmd(split("QUIT :the client on the other end of the link died", " "), cs);
	else if (text.back() == '\n' && text.size() > 1)
		commands = parse_text(text);
	for (size_t i = 0; i < commands.size(); ++i)
	{
		cmd = split(commands[i], " ");
		if (cmd.size() >= 3 && cmd[2].front() == ':')
			 cmd[2].erase(0, 1);
		irc->choose_cmd(cmd, cs);
		cmd.clear();
	}
}
