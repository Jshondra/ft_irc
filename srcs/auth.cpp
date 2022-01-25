#include "../includes/IRC.hpp"

bool IRC::nickname_repl(const std::string& nick) {
	for (int i = 4; i != this->maxfd; ++i)
	{
		if (this->fds[i].getNickname() == nick)
			return true;
	}
	return false;
}

bool IRC::nickname_checker(const std::string& nick) {
	for (size_t i = 0; i != nick.size(); ++i)
	{
		if (!isalnum(nick[i]))
			return true;
	}
	return false;
}

void	IRC::nick_cmd(std::vector<std::string> cmd, int cs)
{
	std::string	nick;
	
	nick = cmd[1];
	if (nickname_repl(nick)) {
		answer_server(cs, 433, nick, "");
		return ;
	}
	if (nickname_checker(nick)) {
		answer_server(cs, 432, nick, "");
		return ;
	}
	if (this->fds[cs].getAuth()) {
		nickname_changer(nick , cs);
		return ;
	}
	this->fds[cs].setNickname(nick);
	if (this->fds[cs].getUsername() != "")
		this->authorization(cs);
}

void	IRC::nickname_changer(std::string nick, int cs){
	std::string answer;
	std::vector<std::string> channels;
	std::vector<int> chnl_clients;

	answer = ":" + this->fds[cs].getNickname() + " NICK :" + nick + "\n";
	channels = this->fds[cs].getChannels();
	this->fds[cs].setNickname(nick);
	for (size_t i = 0; i < channels.size(); i++){
		chnl_clients = this->channels[this->get_fd_channel(channels[i])].get_users();
		for (size_t j = 0; j < chnl_clients.size(); j++) {
			if (chnl_clients[j] != cs) {
				answer_to_client(chnl_clients[j], (char *)answer.c_str());
			}
		}	
	}
}


void	IRC::user_cmd(std::vector<std::string> cmd, int cs)
{
	std::string realname;

	if (cmd.size() < 5) {
		answer_server(cs, 461, this->fds[cs].getNickname(), cmd[0]);
		return ;
	}
	realname = strjoin_cmd(cmd, " ", 4, cmd.size());
	if (realname.front() == ':')
		realname.erase(0, 1);
	this->fds[cs].setRealname(realname);
	this->fds[cs].setUsername(cmd[1]);
	if (this->fds[cs].getNickname() != "" && this->fds[cs].getAuth() == false)
		this->authorization(cs);
}

void	IRC::pass_cmd(std::vector<std::string> cmd, int cs)
{
	std::string	pass;
	
	if (cmd[1][0] == ':')
		cmd[1].erase(0, 1);
	this->fds[cs].setPassword(cmd[1]);
}

void IRC::method(int cs, std::string nick) {
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
	answer_to_client(cs, (char *)(":" + this->servername + " 376 " + nick + " :End of /method command\n").c_str());
}

void	IRC::authorization(int cs){
	if (this->fds[cs].getPassword() != this->irc_pass){
		close(cs);
		this->client_drop(cs);
		std::cout << "Client " << cs << " gone  away\n";
	}
	else {
		this->fds[cs].setAuth(true);
		this->fds[cs].setRegTime(time(0));
		method(cs, this->fds[cs].getNickname());
	}
}
