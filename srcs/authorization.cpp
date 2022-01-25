#include "../includes/Server.hpp"

bool Server::nickname_repl(const std::string& nick) {
	for (int i = 4; i != this->fd_maximum; ++i)
	{
		if (this->file_descriptors[i].getNickname() == nick)
			return true;
	}
	return false;
}

bool Server::nickname_checker(const std::string& nick) {
	for (size_t i = 0; i != nick.size(); ++i)
	{
		if (!isalnum(nick[i]))
			return true;
	}
	return false;
}

void	Server::nick_cmd(std::vector<std::string> cmd, int mc)
{
	std::string	nick;
	
	nick = cmd[1];
	if (nickname_repl(nick)) {
		answer_from_serv(mc, 433, nick, "");
		return ;
	}
	if (nickname_checker(nick)) {
		answer_from_serv(mc, 432, nick, "");
		return ;
	}
	if (this->file_descriptors[mc].getAuth()) {
		nickname_changer(nick , mc);
		return ;
	}
	this->file_descriptors[mc].setNickname(nick);
	if (this->file_descriptors[mc].getUsername() != "")
		this->make_auth(mc);
}

void	Server::nickname_changer(std::string nick, int mc){
	std::string answer;
	std::vector<std::string> all_chns;
	std::vector<int> chnl_clients;

	answer = ":" + this->file_descriptors[mc].getNickname() + " NICK :" + nick + "\n";
	all_chns = this->file_descriptors[mc].getChannels();
	this->file_descriptors[mc].setNickname(nick);
	for (size_t i = 0; i < all_chns.size(); i++){
		chnl_clients = this->all_chns[this->getFdChn(all_chns[i])].getClients();
		for (size_t j = 0; j < chnl_clients.size(); j++) {
			if (chnl_clients[j] != mc) {
				server_answering(chnl_clients[j], (char *)answer.c_str());
			}
		}	
	}
}


void	Server::user_cmd(std::vector<std::string> cmd, int mc)
{
	std::string realname;

	if (cmd.size() < 5) {
		answer_from_serv(mc, 461, this->file_descriptors[mc].getNickname(), cmd[0]);
		return ;
	}
	realname = strjoin_cmd(cmd, " ", 4, cmd.size());
	if (realname.front() == ':')
		realname.erase(0, 1);
	this->file_descriptors[mc].setRealname(realname);
	this->file_descriptors[mc].setUsername(cmd[1]);
	if (this->file_descriptors[mc].getNickname() != "" && this->file_descriptors[mc].getAuth() == false)
		this->make_auth(mc);
}

void	Server::pass_cmd(std::vector<std::string> cmd, int mc)
{
	std::string	pass;
	
	if (cmd[1][0] == ':')
		cmd[1].erase(0, 1);
	this->file_descriptors[mc].setPassword(cmd[1]);
}

void Server::method(int mc, std::string nick) {
	std::string message = ":" + this->server_name + " 372 " + nick;
	server_answering(mc, (char *)(":" + this->server_name + " 375 " + nick + " :- " + this->server_name +
			" Message of the day -\n").c_str());
server_answering(mc, (char *)(message + ":-                                \n").c_str());
server_answering(mc, (char *)(message + " :-  ▄█████▄              ▄██████▄\n").c_str());
server_answering(mc, (char *)(message + " :- █████████▄           ██████████\n").c_str());
server_answering(mc, (char *)(message + " :- ██████████           ██████████\n").c_str());
server_answering(mc, (char *)(message + " :- ██████████▄▄▄▄▄▄▄▄▄▄▄██████████\n").c_str());
server_answering(mc, (char *)(message + " :- ███████████▀▀▀███▀▀▀███████████\n").c_str());
server_answering(mc, (char *)(message + " :- █████████▀░░░░░▀░░░░░▀█████████\n").c_str());
server_answering(mc, (char *)(message + " :-  ▀██████░░░▄▀▀▄░▄▀▀▄░░░██████▀ \n").c_str());
server_answering(mc, (char *)(message + " :-      ▄██░░░█░▄█░█▄░█░░░██▄     \n").c_str());
server_answering(mc, (char *)(message + " :-     ████░░░█░██░██░█░░░████    \n").c_str());
server_answering(mc, (char *)(message + " :-     ████░░░▀▄██▄██▄▀░░░████    \n").c_str());
server_answering(mc, (char *)(message + " :-     ████▄░░▀░░░░░░░▀░░▄████    \n").c_str());
server_answering(mc, (char *)(message + " :-     ██▀░░░░░▄█████▄░░░░░▀██    \n").c_str());
server_answering(mc, (char *)(message + " :-     █░░░░░░░▀█████▀░░░░░░░█    \n").c_str());
server_answering(mc, (char *)(message + " :-     █░░░░▄░░░░▀▀▀░░░░░▄░░░█    \n").c_str());
server_answering(mc, (char *)(message + " :-     ▀█░░░░▀▄▄▄▄▄▄▄▄▄▄▀░░░█▀    \n").c_str());
server_answering(mc, (char *)(message + " :-      ▀▀▀█▄░░▀▄▄▄▄▄▄▀░░▄█▀▀     \n").c_str());
server_answering(mc, (char *)(message + " :-          ▀▀█▄▄▄▄▄▄▄▄█▀▀        \n").c_str());
server_answering(mc, (char *)(message + " :-                                \n").c_str());
server_answering(mc, (char *)(":" + this->server_name + " 376 " + nick + " :End of /method command\033[0m\n").c_str());
}

void	Server::make_auth(int mc){
	if (this->file_descriptors[mc].getPassword() != this->password_server){
		close(mc);
		this->client_drop(mc);
		std::cout << "Client " << mc << " left server\n" << std::endl;
	}
	else {
		this->file_descriptors[mc].setAuth(true);
		this->file_descriptors[mc].setRegTime(time(0));
		method(mc, this->file_descriptors[mc].getNickname());
	}
}
