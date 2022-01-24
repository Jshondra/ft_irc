#include "../includes/IRC.hpp"


void	IRC::ping(std::vector<std::string> cmd, int cs){
	(void)cmd;
	std::string pong = "PONG ";
	std::stringstream t_now;

	t_now << time(0);
	pong.append(t_now.str() + "\n");
	answer_to_client(cs, (char *)pong.c_str());
}

void	IRC::ison(std::vector<std::string> cmd, int cs){
	(void)cmd;
	int cs_auth;

	if (!this->fds[cs].get_auth())
		answer_server(cs, 451, "", "");
	else {
		std::string answer = ":" + this->servername + " 303 " + this->fds[cs].get_nick() + " :";
		for (size_t i = 1; i < cmd.size(); i++) {
			cs_auth = this->get_fd(cmd[i]);
			if (cs_auth != -1 && this->fds[cs_auth].get_auth()) {
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

int	IRC::get_fd_channel(std::string name){
	for (int i = 0; i < MAX_CHANNELS; i++) {
		if (this->channels[i].get_name_channel() == name){
			return i;
		}
	}
	return -1;
}

void	IRC::join(std::vector<std::string> cmd, int cs){
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
		join_channel(cs, cmd[i], password);
	}

}

void	IRC::join_channel(int cs, std::string name, std::string pass){
	int i;


	i = get_fd_channel(name);
	if (i != -1) {
		if (pass == this->channels[i].get_password()) {
			std::vector<int> clients = this->channels[i].get_users();
			for (size_t j = 0; j < clients.size(); j++) {
				if (cs == clients[j]) {
					return ;
				}
			}
			std::cout << "!JOIN! channel found #" << i << " name: " << name <<" whith password " << pass << std::endl;
			this->fds[cs].add_channel(name);
			this->channels[i].add_not_moder(cs);
			after_join(cs, i);
		}
		else {
			answer_server(cs, 475, this->fds[cs].get_nick(), name);
		}
	}
	else {
		i = get_fd_channel("");
		if (i != -1) {
			std::cout << "!JOIN! channel created #" << i << " name: " << name  << " whith password " << pass << std::endl;
			this->channels[i].set_name_channel(name);
			this->channels[i].set_password(pass);
			this->fds[cs].add_channel(name);
			this->channels[i].add_moder(cs);
			after_join(cs, i);
		}
		else {
			std::cout << "WRONG not space for Channels!" << std::endl;
		}
	}
}

void	IRC::after_join(int cs, int chn){
	std::string mess = ":" + this->fds[cs].get_nick() + "!" + this->fds[cs].get_user() + "@" + this->host + " JOIN :" + this->channels[chn].get_name_channel() + "\n";
	std::vector<int> clients = this->channels[chn].get_users();
	for (size_t i = 0; i < clients.size(); i++) {
		answer_to_client(clients[i], (char *)mess.c_str());
	}
	if (this->channels[chn].get_topic() == "")
		answer_server(cs, 331, this->fds[cs].get_nick(), this->channels[chn].get_name_channel());
	else {
		std::string answer = ":" + this->fds[cs].get_nick() + "!" + this->fds[cs].get_user() + this->host + " TOPIC " + this->channels[chn].get_name_channel() + " :" + this->channels[chn].get_topic() + "\n";
		this->answer_to_client(cs, (char *)answer.c_str());
	}
	
	join_participants(cs, chn);
	answer_server(cs, 366, this->fds[cs].get_nick(), this->channels[chn].get_name_channel());
}

void	IRC::join_participants(int cs, int chn){
	std::string mess = ":" + this->servername + " 353 " + this->fds[cs].get_nick() + " = " + this->channels[chn].get_name_channel() + " :";
	std::vector<int> clients = this->channels[chn].get_list_moder();
	for (size_t i = 0; i < clients.size(); i++) {
		mess.append("@");
		mess.append(this->fds[clients[i]].get_nick());
		mess.append(" ");
	}
	clients = this->channels[chn].get_list_not_moder();
	for (size_t i = 0; i < clients.size(); i++) {
		mess.append(this->fds[clients[i]].get_nick());
		if (i + 1 != clients.size())
			mess.append(" ");
	}
	mess.append("\n");
	answer_to_client(cs, (char *)mess.c_str());
}

void	IRC::names(std::vector<std::string> cmd, int cs){
	std::string answer;
	int found;
	int count_found;
	
	if (cmd.size() == 1) {
		answer = "*";
		std::string name_channel;
		for (int i = 0; i < MAX_CHANNELS; i++) {
			name_channel = this->channels[i].get_name_channel();
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
		answer_server(cs, 366, this->fds[cs].get_nick(), answer);
}

void	IRC::who(std::vector<std::string> cmd, int cs){
	(void)cmd;
	answer_server(cs, 315, this->fds[cs].get_nick(), this->fds[cs].get_nick());
}

void	IRC::whois(std::vector<std::string> cmd, int cs){
	int cs_auth;
	std::stringstream t_reg;
	std::stringstream t_now;
		
	cs_auth = this->get_fd(cmd[1]);
	if (cs_auth != -1 && this->fds[cs_auth].get_auth()) {
		std::string mess;
		mess = ":" + this->servername + " 311 " + this->fds[cs].get_nick() + " " + cmd[1] + " " + this->fds[cs_auth].get_user() + " " + this->host + " * :" + this->fds[cs_auth].get_realname() + "\n";
		answer_to_client(cs, (char *)mess.c_str());
		mess = ":" + this->servername + " 319 " + this->fds[cs].get_nick() + " " + cmd[1]  + this->fds[cs_auth].get_channels_string();
		answer_to_client(cs, (char *)mess.c_str());
		answer_server(cs, 312, this->fds[cs].get_nick(), cmd[1]);
		t_now << time(0);
		t_reg << time(0) - this->fds[cs].get_reg_time();
		answer_server(cs, 317, this->fds[cs].get_nick(), cmd[1] + " " + t_reg.str() + " " + t_now.str());
		answer_server(cs, 318, this->fds[cs].get_nick(), cmd[1]);
	}
}

void	IRC::part(std::vector<std::string> cmd, int cs){
	std::string answer;
	for (size_t i = 1; i < cmd.size(); i++) {
		answer = ":" + this->fds[cs].get_nick() + "!" + this->fds[cs].get_user() + this->host + " PART " + cmd[i] + "\n";
		answer_to_client(cs, (char *)answer.c_str());
		this->part_for_each(cmd[i], cs, answer);
	}
}

void	IRC::part_for_each(std::string name, int cs, std::string answer){
	int fd_chn;

	fd_chn = this->get_fd_channel(name);
	if (fd_chn != -1) {
		if (this->fds[cs].delete_cannel(name)){
			answer_server(cs, 442, this->fds[cs].get_nick(), name);
			return ;
		}
		int new_lead = this->channels[fd_chn].leaving_particimant(cs);
		if (new_lead >= 0){
			std::vector<int> particimants_list = this->channels[fd_chn].get_users();
			for (size_t j = 0; j < particimants_list.size(); j++) {
				answer_to_client(particimants_list[j], (char *)answer.c_str());
			}
			if (new_lead != 0) {
				answer = ":" + this->fds[cs].get_nick() + "!" + this->fds[cs].get_user() + this->host + " MODE " + name + " +o " + this->fds[new_lead].get_nick() + "\n";
				for (size_t j = 0; j < particimants_list.size(); j++) {
				answer_to_client(particimants_list[j], (char *)answer.c_str());
				}
			}
		}
	}
	else {
		answer_server(cs, 403, this->fds[cs].get_nick(), name);
	}
}

void	IRC::topic(std::vector<std::string> cmd, int cs) {
	std::vector<std::string> chn = this->fds[cs].get_channels();
	size_t i = 0;
	std::string answer;
	std::vector<int> moder;

	for (; i < chn.size(); i++) {
		if (cmd[1] == chn[i])
			break ;
	}
	if (i == chn.size()) {
		answer_server(cs, 442, this->fds[cs].get_nick(), cmd[1]);
		return ;
	}
	int fd = this->get_fd_channel(cmd[1]);
	if (cmd.size() == 2) {
		if (this->channels[fd].get_topic() == "") {
			answer_server(cs, 331, this->fds[cs].get_nick(), cmd[1]);
		}
		else {
			answer = ":" + this->fds[cs].get_nick() + "!" + this->fds[cs].get_user() + this->host + " TOPIC " + cmd[1] + " :" + this->channels[fd].get_topic() + "\n";
			this->answer_to_client(cs, (char *)answer.c_str());
		}
		return ;
	}
	moder = this->channels[fd].get_list_moder();
	for (size_t i = 0; i < moder.size(); i++) {
		if (cs == moder[i]) {
			answer = strjoin(cmd, " ", 2, cmd.size());
			std::cout << answer << std::endl;
			this->channels[fd].set_topic(answer);
			return ;
		}
	}
	answer_server(cs, 482, this->fds[cs].get_nick(), cmd[1]);
	
}

void	IRC::quit(std::vector<std::string> cmd, int cs) {
	std::string answer;
	std::vector<std::string> channels;

	answer = ":" + this->fds[cs].get_nick() + " QUIT :";
	if (cmd.size() < 2) {
		answer.append("Leaving.");
	}
	else {
		answer += strjoin(cmd, " ", 1, cmd.size());
	}
	answer.append("\n");
	channels = this->fds[cs].get_channels(); 
	for (size_t i = 0; i < channels.size(); i++){
		this->part_for_each(channels[i], cs, answer);
	}
	delete_client(cs);
	close(cs);
}

void	IRC::accept_file(std::vector<std::string> cmd){
	std::cout << "accept_file!\n";
	std::string name = "";
	size_t i = 4;
	

	if (cmd[4][0] != '"')
		return ;
	for (; i  < cmd.size(); i++) {
		if (cmd[i][cmd[i].length() - 1] == '"') {
			name = strjoin(cmd, " ", 4, i + 1);
			break ;
		}
	}
	if (i > cmd.size() - 3)
		return ;
	name.erase(0, 1);
	name.erase(name.length() - 1, 1);
	std::cout << "name = " << name << " ip = " << cmd[i + 1] << " port = " << cmd[i + 2] << " size = " << cmd[i + 3] << std::endl;
	accept_write(name, atoi(cmd[i + 2].c_str()));
}

void	IRC::accept_write(std::string name, int port){
	int			s;
	struct sockaddr_in	sin;

	std::ofstream MyFile(name);
	s = socket(PF_INET, SOCK_STREAM, 0);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	connect(s, (struct sockaddr*)&sin, sizeof(sin));
	MyFile << get_text(s);
	MyFile.close();
	close(s);
}

void	IRC::kick(std::vector<std::string> cmd, int cs) {
	std::vector<std::string> chn = this->fds[cs].get_channels();
	size_t i = 0;
	std::string answer;
	std::vector<int> moder;
	std::vector<std::string> users;

	for (; i < chn.size(); i++) {
		if (cmd[1] == chn[i])
			break ;
	}
	if (i == chn.size()) {
		answer_server(cs, 442, this->fds[cs].get_nick(), cmd[1]);
		return ;
	}
	int fd = this->get_fd_channel(cmd[1]);
	users = split(cmd[2], ",");
	moder = this->channels[fd].get_list_moder();
	for (size_t i = 0; i < moder.size(); i++) {
		if (cs == moder[i]) {
			for (size_t j = 0; j < users.size(); j++) {
				kick_name(cs, cmd, fd, users[j]);	
			}
			return ;
		}
	}
	answer_server(cs, 482, this->fds[cs].get_nick(), cmd[1]);
	
}

void IRC::kick_name(int cs, std::vector<std::string> cmd, int fd_chn, std::string name){
	std::vector<int> users;
	size_t i = 0;

	users = this->channels[fd_chn].get_list_not_moder();
	for (; i < users.size(); i++) {
		if (this->fds[users[i]].get_nick() == name) {
			this->fds[users[i]].delete_cannel(cmd[1]);
			this->channels[fd_chn].leaving_particimant(users[i]);
			break ;
		}
	}
	if (i == users.size()){
		return ;
	}
	std::string answer = ":" + this->fds[cs].get_nick() + "!" + this->fds[cs].get_user() + "@" + this->host + " KICK " + cmd[1] + " " + name + " ";
	if (cmd.size() < 4)
		answer.append(":" + name);
	else
		answer += strjoin(cmd, " ", 3, cmd.size());
	answer.append("\n");
	answer_to_client(users[i], (char *)answer.c_str());

	users = this->channels[fd_chn].get_users();
	for (size_t i = 0; i < users.size(); i++) {
		answer_to_client(users[i], (char *)answer.c_str());
	}
}