#include "../includes/Server.hpp"

# define OFF_COLOR "\033[0m"

void	Server::ping_cmd(std::vector<std::string> cmd, int cs){
	(void)cmd;
	std::string pong = "PONG ";
	std::stringstream t_now;

	t_now << time(0);
	pong.append(t_now.str() + "\n");
	answer_to_client(cs, (char *)pong.c_str());
}

void	Server::ison_cmd(std::vector<std::string> cmd, int cs){
	(void)cmd;
	int cs_auth;

	if (!this->fds[cs].getAuth())
		answer_server(cs, 451, "", "");
	else {
		std::string answer = ":" + this->servername + " 303 " + this->fds[cs].getNickname() + " :";
		for (size_t i = 1; i < cmd.size(); i++) {
			cs_auth = this->get_fd(cmd[i]);
			if (cs_auth != -1 && this->fds[cs_auth].getAuth()) {
				answer.append(cmd[i]);
				if (i + 1 != cmd.size()) {
					answer.append(" ");
				}
			}
		}
		answer.append("\n");
		answer_to_client(cs, (char *)answer.c_str());
	}
}

int	Server::get_fd_channel(std::string name){
	for (int i = 0; i < MAX_CHANNELS; i++) {
		if (this->channels[i].getChannelName() == name){
			return i;
		}
	}
	return -1;
}

void	Server::join_cmd(std::vector<std::string> cmd, int cs){
	size_t pass;
	std::string password;

	pass = 0;
	for (size_t i = 1; i < cmd.size(); i++) {
		if (cmd[i][0] != '#' && cmd[i][0] != '&') {
			pass = i;
			break ;
		}
	}
	if (pass == 1)
		return ;
	if (pass == 0) {
		pass = cmd.size();
	}
	for (size_t i = 1; i < pass; i++) {
		password = pass + i - 1 < cmd.size() ? cmd[pass + i - 1] : "";
		ch_joiner(cs, cmd[i], password);
	}

}

void	Server::ch_joiner(int cs, std::string name, std::string pass){
	int i;


	i = get_fd_channel(name);
	if (i != -1) {
		if (pass == this->channels[i].getPassword()) {
			std::vector<int> clients = this->channels[i].getClients();
			for (size_t j = 0; j < clients.size(); j++) {
				if (cs == clients[j]) {
					return ;
				}
			}
			std::cout << "\033[0;32m!JOIN! channel found #" << i << " name: " << name <<" whith password " << pass << OFF_COLOR << std::endl;
			this->fds[cs].addChannel(name);
			this->channels[i].add_users(cs);
			after_join_cmd(cs, i);
		}
		else {
			answer_server(cs, 475, this->fds[cs].getNickname(), name);
		}
	}
	else {
		i = get_fd_channel("");
		if (i != -1) {
			std::cout << "\033[0;32m!JOIN! channel created #" << i << " name: " << name  << " whith password " << pass << OFF_COLOR << std::endl;
			this->channels[i].setChannelName(name);
			this->channels[i].setPassword(pass);
			this->fds[cs].addChannel(name);
			this->channels[i].add_moderator(cs);
			after_join_cmd(cs, i);
		}
		else {
			std::cout << "\033[0;31mWRONG not space for Channels!" << OFF_COLOR << std::endl;
		}
	}
}

void	Server::after_join_cmd(int cs, int chn){
	std::string mess = ":" + this->fds[cs].getNickname() + "!" + this->fds[cs].getUsername() + "@" + this->host + " JOIN :" + this->channels[chn].getChannelName() + "\n";
	std::vector<int> clients = this->channels[chn].getClients();
	for (size_t i = 0; i < clients.size(); i++) {
		answer_to_client(clients[i], (char *)mess.c_str());
	}
	if (this->channels[chn].getTopic() == "")
		answer_server(cs, 331, this->fds[cs].getNickname(), this->channels[chn].getChannelName());
	else {
		std::string answer = ":" + this->fds[cs].getNickname() + "!" + this->fds[cs].getUsername() + this->host + " TOPIC " + this->channels[chn].getChannelName() + " :" + this->channels[chn].getTopic() + "\n";
		this->answer_to_client(cs, (char *)answer.c_str());
	}
	
	join_participants(cs, chn);
	answer_server(cs, 366, this->fds[cs].getNickname(), this->channels[chn].getChannelName());
}

void	Server::join_participants(int cs, int chn){
	std::string mess = ":" + this->servername + " 353 " + this->fds[cs].getNickname() + " = " + this->channels[chn].getChannelName() + " :";
	std::vector<int> clients = this->channels[chn].getModerator();
	for (size_t i = 0; i < clients.size(); i++) {
		mess.append("@");
		mess.append(this->fds[clients[i]].getNickname());
		mess.append(" ");
	}
	clients = this->channels[chn].getUsers();
	for (size_t i = 0; i < clients.size(); i++) {
		mess.append(this->fds[clients[i]].getNickname());
		if (i + 1 != clients.size())
			mess.append(" ");
	}
	mess.append("\n");
	answer_to_client(cs, (char *)mess.c_str());
}

void	Server::names_cmd(std::vector<std::string> cmd, int cs){
	std::string answer;
	int found;
	int count_found;
	
	if (cmd.size() == 1) {
		answer = "*";
		std::string name_channel;
		for (int i = 0; i < MAX_CHANNELS; i++) {
			name_channel = this->channels[i].getChannelName();
			if (name_channel != "") {
				cmd.push_back(name_channel);
			}
		}
	}
	count_found = 0;
	for (size_t i = 1; i < cmd.size(); i++) {
		
		found = this->get_fd_channel(cmd[i]);
		if (found != -1) {
			this->join_participants(cs, found);
			count_found++;
			if (answer != "*") {
				answer.append(cmd[i]);
				answer.append(" ");
			}
		}
	}
	if (count_found)
		answer_server(cs, 366, this->fds[cs].getNickname(), answer);
}

void	Server::who_cmd(std::vector<std::string> cmd, int cs){
	(void)cmd;
	answer_server(cs, 315, this->fds[cs].getNickname(), this->fds[cs].getNickname());
}

void	Server::whois_cmd(std::vector<std::string> cmd, int cs){
	int cs_auth;
	std::stringstream t_reg;
	std::stringstream t_now;
		
	cs_auth = this->get_fd(cmd[1]);
	if (cs_auth != -1 && this->fds[cs_auth].getAuth()) {
		std::string mess;
		mess = ":" + this->servername + " 311 " + this->fds[cs].getNickname() + " " + cmd[1] + " " + this->fds[cs_auth].getUsername() + " " + this->host + " * :" + this->fds[cs_auth].getRealname() + "\n";
		answer_to_client(cs, (char *)mess.c_str());
		mess = ":" + this->servername + " 319 " + this->fds[cs].getNickname() + " " + cmd[1]  + this->fds[cs_auth].getChannelsStr();
		answer_to_client(cs, (char *)mess.c_str());
		answer_server(cs, 312, this->fds[cs].getNickname(), cmd[1]);
		t_now << time(0);
		t_reg << time(0) - this->fds[cs].getRegTime();
		answer_server(cs, 317, this->fds[cs].getNickname(), cmd[1] + " " + t_reg.str() + " " + t_now.str());
		answer_server(cs, 318, this->fds[cs].getNickname(), cmd[1]);
	}
}

void	Server::part_cmd(std::vector<std::string> cmd, int cs){
	std::string answer;
	for (size_t i = 1; i < cmd.size(); i++) {
		answer = ":" + this->fds[cs].getNickname() + "!" + this->fds[cs].getUsername() + this->host + " PART " + cmd[i] + "\n";
		answer_to_client(cs, (char *)answer.c_str());
		this->selecter_each(cmd[i], cs, answer);
	}
}

void	Server::selecter_each(std::string name, int cs, std::string answer){
	int fd_chn;

	fd_chn = this->get_fd_channel(name);
	if (fd_chn != -1) {
		if (this->fds[cs].deleteChannel(name)){
			answer_server(cs, 442, this->fds[cs].getNickname(), name);
			return ;
		}
		int new_lead = this->channels[fd_chn].leaving_particimant(cs);
		if (new_lead >= 0){
			std::vector<int> particimants_list = this->channels[fd_chn].getClients();
			for (size_t j = 0; j < particimants_list.size(); j++) {
				answer_to_client(particimants_list[j], (char *)answer.c_str());
			}
			if (new_lead != 0) {
				answer = ":" + this->fds[cs].getNickname() + "!" + this->fds[cs].getUsername() + this->host + " MODE " + name + " +o " + this->fds[new_lead].getNickname() + "\n";
				for (size_t j = 0; j < particimants_list.size(); j++) {
				answer_to_client(particimants_list[j], (char *)answer.c_str());
				}
			}
		}
	}
	else {
		answer_server(cs, 403, this->fds[cs].getNickname(), name);
	}
}

void	Server::topic_cmd(std::vector<std::string> cmd, int cs) {
	std::vector<std::string> chn = this->fds[cs].getChannels();
	size_t i = 0;
	std::string answer;
	std::vector<int> moder;

	for (; i < chn.size(); i++) {
		if (cmd[1] == chn[i])
			break ;
	}
	if (i == chn.size()) {
		answer_server(cs, 442, this->fds[cs].getNickname(), cmd[1]);
		return ;
	}
	int fd = this->get_fd_channel(cmd[1]);
	if (cmd.size() == 2) {
		if (this->channels[fd].getTopic() == "") {
			answer_server(cs, 331, this->fds[cs].getNickname(), cmd[1]);
		}
		else {
			answer = ":" + this->fds[cs].getNickname() + "!" + this->fds[cs].getUsername() + this->host + " TOPIC " + cmd[1] + " :" + this->channels[fd].getTopic() + "\n";
			this->answer_to_client(cs, (char *)answer.c_str());
		}
		return ;
	}
	moder = this->channels[fd].getModerator();
	for (size_t i = 0; i < moder.size(); i++) {
		if (cs == moder[i]) {
			answer = strjoin_cmd(cmd, " ", 2, cmd.size());
			std::cout << answer << std::endl;
			this->channels[fd].setTopic(answer);
			return ;
		}
	}
	answer_server(cs, 482, this->fds[cs].getNickname(), cmd[1]);
	
}

void	Server::quit_cmd(std::vector<std::string> cmd, int cs) {
	std::string answer;
	std::vector<std::string> channels;

	answer = ":" + this->fds[cs].getNickname() + " QUIT :";
	if (cmd.size() < 2) {
		answer.append("Leaving.");
	}
	else {
		answer += strjoin_cmd(cmd, " ", 1, cmd.size());
	}
	answer.append("\n");
	channels = this->fds[cs].getChannels(); 
	for (size_t i = 0; i < channels.size(); i++){
		this->selecter_each(channels[i], cs, answer);
	}
	client_drop(cs);
	close(cs);
}

void	Server::kick_cmd(std::vector<std::string> cmd, int cs) {
	std::vector<std::string> chn = this->fds[cs].getChannels();
	size_t i = 0;
	std::string answer;
	std::vector<int> moder;
	std::vector<std::string> users;

	for (; i < chn.size(); i++) {
		if (cmd[1] == chn[i])
			break ;
	}
	if (i == chn.size()) {
		answer_server(cs, 442, this->fds[cs].getNickname(), cmd[1]);
		return ;
	}
	int fd = this->get_fd_channel(cmd[1]);
	users = our_own_split(cmd[2], ",");
	moder = this->channels[fd].getModerator();
	for (size_t i = 0; i < moder.size(); i++) {
		if (cs == moder[i]) {
			for (size_t j = 0; j < users.size(); j++) {
				kick_name(cs, cmd, fd, users[j]);	
			}
			return ;
		}
	}
	answer_server(cs, 482, this->fds[cs].getNickname(), cmd[1]);
	
}

void Server::kick_name(int cs, std::vector<std::string> cmd, int fd_chn, std::string name){
	std::vector<int> users;
	size_t i = 0;

	users = this->channels[fd_chn].getUsers();
	for (; i < users.size(); i++) {
		if (this->fds[users[i]].getNickname() == name) {
			this->fds[users[i]].deleteChannel(cmd[1]);
			this->channels[fd_chn].leaving_particimant(users[i]);
			break ;
		}
	}
	if (i == users.size()){
		return ;
	}
	std::string answer = ":" + this->fds[cs].getNickname() + "!" + this->fds[cs].getUsername() + "@" + this->host + " KICK " + cmd[1] + " " + name + " ";
	if (cmd.size() < 4)
		answer.append(":" + name);
	else
		answer += strjoin_cmd(cmd, " ", 3, cmd.size());
	answer.append("\n");
	answer_to_client(users[i], (char *)answer.c_str());

	users = this->channels[fd_chn].getClients();
	for (size_t i = 0; i < users.size(); i++) {
		answer_to_client(users[i], (char *)answer.c_str());
	}
}