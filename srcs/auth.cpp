#include "../includes/IRC.hpp"

bool IRC::replay_nick(const std::string& nick) {
	for (int i = 4; i != this->maxfd; ++i)
	{
		if (this->fds[i].get_nick() == nick)
			return true;
	}
	return false;
}

bool IRC::check_nick(const std::string& nick) {
	for (size_t i = 0; i != nick.size(); ++i)
	{
		if (!isalnum(nick[i]))
			return true;
	}
	return false;
}

void	IRC::nick(std::vector<std::string> cmd, int cs)
{
	std::string	nick;
	
	nick = cmd[1];
	if (replay_nick(nick)) {
		answer_server(cs, 433, nick, "");
		return ;
	}
	if (check_nick(nick)) {
		answer_server(cs, 432, nick, "");
		return ;
	}
	if (this->fds[cs].get_auth()) {
		change_nick(nick , cs);
		return ;
	}
	this->fds[cs].set_nick(nick);
	if (this->fds[cs].get_user() != "")
		this->authorization(cs);
}

void	IRC::change_nick(std::string nick, int cs){
	std::string answer;
	std::vector<std::string> channels;
	std::vector<int> chnl_clients;

	answer = ":" + this->fds[cs].get_nick() + " NICK :" + nick + "\n";
	channels = this->fds[cs].get_channels();
	this->fds[cs].set_nick(nick);
	for (size_t i = 0; i < channels.size(); i++){
		chnl_clients = this->channels[this->get_fd_channel(channels[i])].get_users();
		for (size_t j = 0; j < chnl_clients.size(); j++) {
			if (chnl_clients[j] != cs) {
				answer_to_client(chnl_clients[j], (char *)answer.c_str());
			}
		}	
	}
}


void	IRC::user(std::vector<std::string> cmd, int cs)
{
	std::string realname;

	if (cmd.size() < 5) {
		answer_server(cs, 461, this->fds[cs].get_nick(), cmd[0]);
		return ;
	}
	realname = strjoin(cmd, " ", 4, cmd.size());
	if (realname.front() == ':')
		realname.erase(0, 1);
	this->fds[cs].set_realname(realname);
	this->fds[cs].set_user(cmd[1]);
	if (this->fds[cs].get_nick() != "" && this->fds[cs].get_auth() == false)
		this->authorization(cs);
}

void	IRC::pass(std::vector<std::string> cmd, int cs)
{
	std::string	pass;
	
	if (cmd[1][0] == ':')
		cmd[1].erase(0, 1);
	this->fds[cs].set_pass(cmd[1]);
}

void IRC::motd(int cs, std::string nick) {
	std::string message = ":" + this->servername + " 372 " + nick;
	answer_to_client(cs, (char *)(":" + this->servername + " 375 " + nick + " :- " + this->servername +
			" Message of the day -\n").c_str());
	answer_to_client(cs, (char *)(message + " :- ⣿⣿⣿⣿⣿⣿⣿⣿⠟⠋⠁⠄⠄⠄⠄⠄⠄⠄⠄⠙⢿⣿⣿⣿⣿⣿⣿⣿⣿\n").c_str());
	answer_to_client(cs, (char *)(message + " :- ⣿⣿⣿⣿⣿⣿⡟⠁⠄⠄⠄⠄⣠⣤⣴⣶⣶⣶⣶⣤⡀⠈⠙⢿⣿⣿⣿⣿⣿\n").c_str());
	answer_to_client(cs, (char *)(message + " :- ⣿⣿⣿⣿⣿⡟⠄⠄⠄⠄⠄⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣆⠄⠈⣿⣿⣿⣿⣿\n").c_str());
	answer_to_client(cs, (char *)(message + " :- ⣿⣿⣿⣿⣿⠁⠄⠄⠄⢀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠄⠄⢺⣿⣿⣿⣿\n").c_str());
	answer_to_client(cs, (char *)(message + " :- ⣿⣿⣿⣿⣿⡄⠄⠄⠄⠙⠻⠿⣿⣿⣿⣿⠿⠿⠛⠛⠻⣿⡄⠄⣾⣿⣿⣿⣿\n").c_str());
	answer_to_client(cs, (char *)(message + " :- ⣿⣿⣿⣿⣿⡇⠄⠄⠁ 👁 ⠄⢹⣿⡗⠄ 👁 ⢄⡀⣾⢀⣿⣿⣿⣿⣿\n").c_str());
	answer_to_client(cs, (char *)(message + " :- ⣿⣿⣿⣿⣿⡇⠘⠄⠄⠄⢀⡀⠄⣿⣿⣷⣤⣤⣾⣿⣿⣿⣧⢸⣿⣿⣿⣿⣿\n").c_str());
	answer_to_client(cs, (char *)(message + " :- ⣿⣿⣿⣿⣿⡇⠄⣰⣿⡿⠟⠃⠄⣿⣿⣿⣿⣿⡛⠿⢿⣿⣷⣾⣿⣿⣿⣿⣿\n").c_str());
	answer_to_client(cs, (char *)(message + " :- ⣿⣿⣿⣿⣿⣿⡄⠈⠁⠄⠄⠄⠄⠻⠿⢛⣿⣿⠿⠂⠄⢹⢹⣿⣿⣿⣿⣿⣿\n").c_str());
	answer_to_client(cs, (char *)(message + " :- ⣿⣿⣿⣿⣿⣿⣿⡐⠐⠄⠄⣠⣀⣀⣚⣯⣵⣶⠆⣰⠄⠞⣾⣿⣿⣿⣿⣿⣿\n").c_str());
	answer_to_client(cs, (char *)(message + " :- ⣿⣿⣿⣿⣿⣿⣿⡐⠐⠄⠄⣠⣀⣀⣚⣯⣵⣶⠆⣰⠄⠞⣾⣿⣿⣿⣿⣿⣿\n").c_str());
	answer_to_client(cs, (char *)(message + " :- ⣿⠿⠿⠋⠉⠄⠄⠄⠄⠄⠄⠄⣀⣠⣾⣿⣿⣿⡟⠁⠹⡇⣸⣿⣿⣿⣿⣿⣿\n").c_str());
	answer_to_client(cs, (char *)(message + " :- ⠁⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠙⠿⠿⠛⠋⠄⣸⣦⣠⣿⣿⣿⣿⣿⣿⣿\n").c_str());
	answer_to_client(cs, (char *)(":" + this->servername + " 376 " + nick + " :End of /MOTD command\n").c_str());
}

void	IRC::authorization(int cs){
	if (this->fds[cs].get_pass() != this->irc_pass){
		close(cs);
		this->delete_client(cs);
		std::cout << "Client " << cs << " gone  away\n";
	}
	else {
		this->fds[cs].set_auth(true);
		this->fds[cs].set_retg_time(time(0));
		motd(cs, this->fds[cs].get_nick());
	}
}
