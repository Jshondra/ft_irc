#include "../includes/Server.hpp"

void Server::user_sender(std::vector<std::string> cmd, int client_fd, int cs, std::string sender, std::string recipient) 
{
	std::string answer;
	std::string message;
	std::string away_message;

	if (!this->fds[client_fd].getAuth())
	{
		answer_server(cs, 401, sender, recipient);
		return ;
	}
	//senf_file
	// if (cmd.size() > 3 && (int)cmd[2].c_str()[1] == 1)
	// 	file_accepter(cmd);
	//send_fule_end
	away_message = this->fds[client_fd].getaway_cmd();
	message = strjoin_cmd(cmd, " ", 2, cmd.size());
	answer = ":" + sender + " " + cmd.front() + " " + recipient + " :" + message + '\n';
	answer_to_client(client_fd, (char *)answer.c_str());
	if (!away_message.empty() && cmd.front() != "NOTICE")
	{
		answer = ":" + recipient + " PRIVMSG " + sender + " :" + away_message + '\n';
		answer_to_client(cs, (char *)answer.c_str());
	}
}

void Server::channel_sender(std::vector<std::string> cmd, int client_fd, int cs, std::string sender, std::string recipient) {
	std::string answer;
	std::string message;

	message = strjoin_cmd(cmd, " ", 2, cmd.size());
	answer = ":" + sender + "!" + this->fds[cs].getUsername() + "@" + this->host + " PRIVMSG " + recipient + " :" +
			message + '\n';
	std::vector<int> users = this->channels[client_fd].get_users();
	for (size_t i = 0; i < users.size(); ++i)
		if (users[i] != cs)
			answer_to_client(users[i], (char *) answer.c_str());
}

void	Server::privmsg_cmd(std::vector<std::string> cmd, int cs)
{
	int			client_fd;
	std::string sender;
	std::string	recipient;

	sender = this->fds[cs].getNickname();
	if (cmd.size() < 3)
	{
		answer_server(cs, 412, sender, "");
		return ;
	}
	recipient = cmd[1];
	if (recipient.front() != '#' && recipient.front() != '&')
		client_fd = this->get_fd(recipient);
	else
		client_fd = this->get_fd_channel(recipient);
	if (client_fd < 0)
		answer_server(cs, 401, sender, recipient);
	else if (recipient.front() != '#' && recipient.front() != '&')
		user_sender(cmd, client_fd, cs, sender, recipient);
	else
		channel_sender(cmd, client_fd, cs, sender, recipient);
}
