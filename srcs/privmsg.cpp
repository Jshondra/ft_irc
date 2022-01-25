#include "../includes/Server.hpp"

void Server::user_sender(std::vector<std::string> cmd, int client_fd, int mc, std::string sender, std::string recipient) 
{
	std::string answer;
	std::string message;
	std::string away_message;

	if (!this->file_descriptors[client_fd].getAuth())
	{
		answer_from_serv(mc, 401, sender, recipient);
		return ;
	}
	away_message = this->file_descriptors[client_fd].getaway_cmd();
	message = strjoin_cmd(cmd, " ", 2, cmd.size());
	answer = ":" + sender + " " + cmd.front() + " " + recipient + " :" + message + '\n';
	server_answering(client_fd, (char *)answer.c_str());
	if (!away_message.empty() && cmd.front() != "NOTICE")
	{
		answer = ":" + recipient + " PRIVMSG " + sender + " :" + away_message + '\n';
		server_answering(mc, (char *)answer.c_str());
	}
}

void	Server::privmsg_cmd(std::vector<std::string> cmd, int mc)
{
	int			client_fd;
	std::string sender;
	std::string	recipient;

	sender = this->file_descriptors[mc].getNickname();
	if (cmd.size() < 3)
	{
		answer_from_serv(mc, 412, sender, "");
		return ;
	}
	recipient = cmd[1];
	if (recipient.front() != '#' && recipient.front() != '&')
		client_fd = this->getFd(recipient);
	else
		client_fd = this->getFdChn(recipient);
	if (client_fd < 0)
		answer_from_serv(mc, 401, sender, recipient);
	else if (recipient.front() != '#' && recipient.front() != '&')
		user_sender(cmd, client_fd, mc, sender, recipient);
	else
		channel_sender(cmd, client_fd, mc, sender, recipient);
}


void Server::channel_sender(std::vector<std::string> cmd, int client_fd, int mc, std::string sender, std::string recipient) {
	std::string answer;
	std::string message;

	message = strjoin_cmd(cmd, " ", 2, cmd.size());
	answer = ":" + sender + "!" + this->file_descriptors[mc].getUsername() + "@" + this->host + " PRIVMSG " + recipient + " :" +
			message + '\n';
	std::vector<int> users = this->all_chns[client_fd].getClients();
	for (size_t i = 0; i < users.size(); ++i)
		if (users[i] != mc)
			server_answering(users[i], (char *) answer.c_str());
}
