#include "../includes/Server.hpp"

# define OFF_COLOR "\033[0m"

Server::Server(std::string host, int port, std::string pass) {
	fd_maximum = MAX_CLIENT;
	file_descriptors = new Client[fd_maximum];
	all_chns = new Channels[MAX_CHANNELS];
	this->port = port;
	this->password_server = pass;
	this->host = host;
	creation_server();
	all_commands();
}

Server::Server(const Server &server) {
	*this = server;
}

Server::~Server(void) {
	delete[] file_descriptors;
	delete[] all_chns;
}

Server&	Server::operator=(const Server &server) {
	if (this != &server){
		fd_maximum = server.fd_maximum;
		file_descriptors = server.file_descriptors;
	}
	return (*this);
}

void	Server::creation_server(void) {
	struct sockaddr_in	sin;

	this->sockets = socket(PF_INET, SOCK_STREAM, 0);
	if (this->sockets == -1) {
		std::cout << "\033[0;31mFailed to create socket. errno: " << errno << OFF_COLOR << std::endl;
		exit (errno);
	}
	const int trueFlag = 1;
	if (setsockopt(this->sockets, SOL_SOCKET, SO_REUSEADDR, &trueFlag, sizeof(int)) < 0) {
		std::cout << "\033[0;31msetsockopt failed" << OFF_COLOR << std::endl;
		exit(errno);
	}
	sin.sin_family = AF_INET;
	this->server_name = SERVERNAME;
	this->hostname = inet_addr((const char *)this->host.c_str());
	sin.sin_addr.s_addr = this->hostname;
	sin.sin_port = htons(this->port);
	if (bind(this->sockets, (struct sockaddr*)&sin, sizeof(sin)) == -1) {
		std::cout << "\033[0;31mFailed to bind to port " << port << OFF_COLOR << std::endl;
		exit(errno);
	}
	if (listen(this->sockets, 21) == -1) {
		std::cout << "\033[0;31mFailed to listen on socket \033[0m" << std::endl;
		exit(104);
	}
	fcntl(this->sockets, F_SETFL, O_NONBLOCK);
	this->file_descriptors[this->sockets].setFdValue(FD_SERV);
	this->file_descriptors[this->sockets].setFctRead(server_accepter);
}

void	server_accepter(Server *irc, int s) {
	int					mc;
	struct sockaddr_in	csin;
	socklen_t			csin_len;

	csin_len = sizeof(csin);
	mc = accept(s, (struct sockaddr*)&csin, &csin_len);
	if (mc > irc->getFdMax())
		close(mc);
	else {
		std::cout << "\033[0;32mNew client " << mc << " from " << inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) << " \033[0m" << std::endl;
		irc->client_adder(mc);
	}
}

void	Server::readCleanBuffer(int mc) {
	file_descriptors[mc].cleanBufRead();
}

char*	Server::getBufferRead(int mc) {
	return file_descriptors[mc].getBufRead();
}

char*	Server::getBufferWrite(int mc) {
	return file_descriptors[mc].getBufWrite();
}

void	writer_client(Server *irc, int mc){
	(void)mc;
	(void)irc;
}

int		Server::getFdMax() {
	return this->fd_maximum;
}

int		Server::getFdValue_client(int mc){
	return this->file_descriptors[mc].getFdValue();
}

void	Server::client_adder(int mc){
	this->file_descriptors[mc].setFdValue(FD_CLIENT);
	this->file_descriptors[mc].setFctRead(reader_client);
	this->file_descriptors[mc].setFctWrite(writer_client);
}

void	Server::client_drop(int mc) {
	this->file_descriptors[mc].cleanClient();
}

void	Server::fd_make() {
	int	i;

	i = 0;
	this->maximum = 0;
	FD_ZERO(&this->readFd);
	FD_ZERO(&this->writeFd);
	while (i < this->getFdMax()){
		if (this->getFdValue_client(i) != FD_FREE){
			FD_SET(i, &this->readFd);
			
			if (strlen(this->getBufferWrite(i)) > 0)
				FD_SET(i, &this->writeFd);
			this->maximum = MAX(this->maximum, i);
			}
		i++;
	}
}

void	Server::select_to_do() {
	this->rack = select(this->maximum + 1, &this->readFd, &this->writeFd, nullptr, nullptr);
}

void	Server::fd_checker() {
	int	i;

	i = 0;
	while ((i < this->fd_maximum) && (this->rack > 0)){
		if (FD_ISSET(i, &this->readFd))
			this->clientIsReading(i);
		if (FD_ISSET(i, &this->writeFd))
			this->clientIsWriting(i);
		if (FD_ISSET(i, &this->readFd) || FD_ISSET(i, &this->writeFd))
			this->rack--;
		i++;
	}
}

void	Server::clientIsReading(int i){
	this->file_descriptors[i].exRead(this, i);
}

void	Server::clientIsWriting(int i){
	this->file_descriptors[i].exWrite(this, i);
}

void Server::all_commands()
{
	this->allCommands["NICK"] = &Server::nick_cmd;
	this->allCommands["USER"] = &Server::user_cmd;
	this->allCommands["PASS"] = &Server::pass_cmd;
	this->allCommands["PRIVMSG"] = &Server::privmsg_cmd;
	this->allCommands["NOTICE"] = &Server::ison_cmd;
	this->allCommands["ISON"] = &Server::ison_cmd;
	this->allCommands["PING"] = &Server::ping_cmd;
	this->allCommands["JOIN"] = &Server::join_cmd;
	this->allCommands["WHO"] = &Server::who_cmd;
	this->allCommands["WHOIS"] = &Server::whois_cmd;
	this->allCommands["PART"] = &Server::part_cmd;
	this->allCommands["AWAY"] = &Server::away_cmd;
	this->allCommands["NAMES"] = &Server::names_cmd;
	this->allCommands["TOPIC"] = &Server::topic_cmd;
	this->allCommands["QUIT"] = &Server::quit_cmd;
	this->allCommands["KICK"] = &Server::kick_cmd;
}

void	Server::command_case(std::vector<std::string> cmd, int mc)
{
	std::map<std::string, void (Server::*)(std::vector<std::string> cmd, int mc)>::iterator found;
    found = this->allCommands.find(cmd.front());
	if (found != allCommands.end())
	{
		if (cmd.size() == 1 && cmd.front() != "AWAY" && cmd.front() != "NAMES" && cmd.front() != "QUIT")
		{
			answer_from_serv(mc, 461, this->file_descriptors[mc].getNickname(), cmd[0]);
			return ;
		}
		if (cmd[0] == "NICK" || cmd[0] == "USER" || cmd[0] == "PASS" || cmd[0] == "QUIT" || this->file_descriptors[mc].getAuth()) {
			(this->*(allCommands[(cmd[0])]))(cmd, mc);
		}
		else 
			answer_from_serv(mc, 451, "", "");
	}
	else {
		if (!this->file_descriptors[mc].getAuth())
			answer_from_serv(mc, 451, "", "");
		else
			answer_from_serv(mc, 421, this->file_descriptors[mc].getNickname(), cmd.front());
	}
}


int		Server::getFd(const std::string& nick) {
	for (int i = 4; i != this->getFdMax(); ++i)
	{
		if (nick == this->file_descriptors[i].getNickname())
			return (i);
	}
	return -1;
}

bool	Server::run_checker(bool run) {
	if (run)
		return true;
	for (int fd = 3; fd < this->fd_maximum; ++fd)
	{
		client_drop(fd);
		close(fd);
	}
	linger opt;
	opt.l_onoff = 1;
	opt.l_linger = 0;
	setsockopt(this->sockets, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt));
	close(this->sockets);
	exit(130);
}

Server::Server() {}


// COMMANDS

void	Server::ping_cmd(std::vector<std::string> cmd, int mc){
	(void)cmd;
	std::string pong = "PONG ";
	std::stringstream t_now;

	t_now << time(0);
	pong.append(t_now.str() + "\n");
	server_answering(mc, (char *)pong.c_str());
}

void	Server::ison_cmd(std::vector<std::string> cmd, int mc){
	(void)cmd;
	int cs_auth;

	if (!this->file_descriptors[mc].getAuth())
		answer_from_serv(mc, 451, "", "");
	else {
		std::string answer = ":" + this->server_name + " 303 " + this->file_descriptors[mc].getNickname() + " :";
		for (size_t i = 1; i < cmd.size(); i++) {
			cs_auth = this->getFd(cmd[i]);
			if (cs_auth != -1 && this->file_descriptors[cs_auth].getAuth()) {
				answer.append(cmd[i]);
				if (i + 1 != cmd.size()) {
					answer.append(" ");
				}
			}
		}
		answer.append("\n");
		server_answering(mc, (char *)answer.c_str());
	}
}

int	Server::getFdChn(std::string name){
	for (int i = 0; i < MAX_CHANNELS; i++) {
		if (this->all_chns[i].getChannelName() == name){
			return i;
		}
	}
	return -1;
}

void	Server::join_cmd(std::vector<std::string> cmd, int mc){
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
		ch_joiner(mc, cmd[i], password);
	}

}

void	Server::ch_joiner(int mc, std::string name, std::string pass){
	int i;


	i = getFdChn(name);
	if (i != -1) {
		if (pass == this->all_chns[i].getPassword()) {
			std::vector<int> clients = this->all_chns[i].getClients();
			for (size_t j = 0; j < clients.size(); j++) {
				if (mc == clients[j]) {
					return ;
				}
			}
			std::cout << "\033[0;32m!JOIN! channel found #" << i << " name: " << name <<" whith password " << pass << OFF_COLOR << std::endl;
			this->file_descriptors[mc].addChannel(name);
			this->all_chns[i].add_users(mc);
			after_join_cmd(mc, i);
		}
		else {
			answer_from_serv(mc, 475, this->file_descriptors[mc].getNickname(), name);
		}
	}
	else {
		i = getFdChn("");
		if (i != -1) {
			std::cout << "\033[0;32m!JOIN! channel created #" << i << " name: " << name  << " whith password " << pass << OFF_COLOR << std::endl;
			this->all_chns[i].setChannelName(name);
			this->all_chns[i].setPassword(pass);
			this->file_descriptors[mc].addChannel(name);
			this->all_chns[i].add_moderator(mc);
			after_join_cmd(mc, i);
		}
		else {
			std::cout << "\033[0;31mWRONG not space for Channels!" << OFF_COLOR << std::endl;
		}
	}
}

void	Server::after_join_cmd(int mc, int chn){
	std::string mess = ":" + this->file_descriptors[mc].getNickname() + "!" + this->file_descriptors[mc].getUsername() + "@" + this->host + " JOIN :" + this->all_chns[chn].getChannelName() + "\n";
	std::vector<int> clients = this->all_chns[chn].getClients();
	for (size_t i = 0; i < clients.size(); i++) {
		server_answering(clients[i], (char *)mess.c_str());
	}
	if (this->all_chns[chn].getTopic() == "")
		answer_from_serv(mc, 331, this->file_descriptors[mc].getNickname(), this->all_chns[chn].getChannelName());
	else {
		std::string answer = ":" + this->file_descriptors[mc].getNickname() + "!" + this->file_descriptors[mc].getUsername() + this->host + " TOPIC " + this->all_chns[chn].getChannelName() + " :" + this->all_chns[chn].getTopic() + "\n";
		this->server_answering(mc, (char *)answer.c_str());
	}
	
	users_join_to_chn(mc, chn);
	answer_from_serv(mc, 366, this->file_descriptors[mc].getNickname(), this->all_chns[chn].getChannelName());
}

void	Server::users_join_to_chn(int mc, int chn){
	std::string mess = ":" + this->server_name + " 353 " + this->file_descriptors[mc].getNickname() + " = " + this->all_chns[chn].getChannelName() + " :";
	std::vector<int> clients = this->all_chns[chn].getModerator();
	for (size_t i = 0; i < clients.size(); i++) {
		mess.append("@");
		mess.append(this->file_descriptors[clients[i]].getNickname());
		mess.append(" ");
	}
	clients = this->all_chns[chn].getUsers();
	for (size_t i = 0; i < clients.size(); i++) {
		mess.append(this->file_descriptors[clients[i]].getNickname());
		if (i + 1 != clients.size())
			mess.append(" ");
	}
	mess.append("\n");
	server_answering(mc, (char *)mess.c_str());
}

void	Server::names_cmd(std::vector<std::string> cmd, int mc){
	std::string answer;
	int found;
	int count_found;
	
	if (cmd.size() == 1) {
		answer = "*";
		std::string name_channel;
		for (int i = 0; i < MAX_CHANNELS; i++) {
			name_channel = this->all_chns[i].getChannelName();
			if (name_channel != "") {
				cmd.push_back(name_channel);
			}
		}
	}
	count_found = 0;
	for (size_t i = 1; i < cmd.size(); i++) {
		
		found = this->getFdChn(cmd[i]);
		if (found != -1) {
			this->users_join_to_chn(mc, found);
			count_found++;
			if (answer != "*") {
				answer.append(cmd[i]);
				answer.append(" ");
			}
		}
	}
	if (count_found)
		answer_from_serv(mc, 366, this->file_descriptors[mc].getNickname(), answer);
}

void	Server::who_cmd(std::vector<std::string> cmd, int mc){
	(void)cmd;
	answer_from_serv(mc, 315, this->file_descriptors[mc].getNickname(), this->file_descriptors[mc].getNickname());
}

void	Server::whois_cmd(std::vector<std::string> cmd, int mc){
	int cs_auth;
	std::stringstream t_reg;
	std::stringstream t_now;
		
	cs_auth = this->getFd(cmd[1]);
	if (cs_auth != -1 && this->file_descriptors[cs_auth].getAuth()) {
		std::string mess;
		mess = ":" + this->server_name + " 311 " + this->file_descriptors[mc].getNickname() + " " + cmd[1] + " " + this->file_descriptors[cs_auth].getUsername() + " " + this->host + " * :" + this->file_descriptors[cs_auth].getRealname() + "\n";
		server_answering(mc, (char *)mess.c_str());
		mess = ":" + this->server_name + " 319 " + this->file_descriptors[mc].getNickname() + " " + cmd[1]  + this->file_descriptors[cs_auth].getChannelsStr();
		server_answering(mc, (char *)mess.c_str());
		answer_from_serv(mc, 312, this->file_descriptors[mc].getNickname(), cmd[1]);
		t_now << time(0);
		t_reg << time(0) - this->file_descriptors[mc].getRegTime();
		answer_from_serv(mc, 317, this->file_descriptors[mc].getNickname(), cmd[1] + " " + t_reg.str() + " " + t_now.str());
		answer_from_serv(mc, 318, this->file_descriptors[mc].getNickname(), cmd[1]);
	}
}

void	Server::part_cmd(std::vector<std::string> cmd, int mc){
	std::string answer;
	for (size_t i = 1; i < cmd.size(); i++) {
		answer = ":" + this->file_descriptors[mc].getNickname() + "!" + this->file_descriptors[mc].getUsername() + this->host + " PART " + cmd[i] + "\n";
		server_answering(mc, (char *)answer.c_str());
		this->selecter_each(cmd[i], mc, answer);
	}
}

void	Server::selecter_each(std::string name, int mc, std::string answer){
	int fd_chn;

	fd_chn = this->getFdChn(name);
	if (fd_chn != -1) {
		if (this->file_descriptors[mc].deleteChannel(name)){
			answer_from_serv(mc, 442, this->file_descriptors[mc].getNickname(), name);
			return ;
		}
		int new_lead = this->all_chns[fd_chn].leaving_particimant(mc);
		if (new_lead >= 0){
			std::vector<int> particimants_list = this->all_chns[fd_chn].getClients();
			for (size_t j = 0; j < particimants_list.size(); j++) {
				server_answering(particimants_list[j], (char *)answer.c_str());
			}
			if (new_lead != 0) {
				answer = ":" + this->file_descriptors[mc].getNickname() + "!" + this->file_descriptors[mc].getUsername() + this->host + " MODE " + name + " +o " + this->file_descriptors[new_lead].getNickname() + "\n";
				for (size_t j = 0; j < particimants_list.size(); j++) {
				server_answering(particimants_list[j], (char *)answer.c_str());
				}
			}
		}
	}
	else {
		answer_from_serv(mc, 403, this->file_descriptors[mc].getNickname(), name);
	}
}

void	Server::topic_cmd(std::vector<std::string> cmd, int mc) {
	std::vector<std::string> chn = this->file_descriptors[mc].getChannels();
	size_t i = 0;
	std::string answer;
	std::vector<int> moder;

	for (; i < chn.size(); i++) {
		if (cmd[1] == chn[i])
			break ;
	}
	if (i == chn.size()) {
		answer_from_serv(mc, 442, this->file_descriptors[mc].getNickname(), cmd[1]);
		return ;
	}
	int fd = this->getFdChn(cmd[1]);
	if (cmd.size() == 2) {
		if (this->all_chns[fd].getTopic() == "") {
			answer_from_serv(mc, 331, this->file_descriptors[mc].getNickname(), cmd[1]);
		}
		else {
			answer = ":" + this->file_descriptors[mc].getNickname() + "!" + this->file_descriptors[mc].getUsername() + this->host + " TOPIC " + cmd[1] + " :" + this->all_chns[fd].getTopic() + "\n";
			this->server_answering(mc, (char *)answer.c_str());
		}
		return ;
	}
	moder = this->all_chns[fd].getModerator();
	for (size_t i = 0; i < moder.size(); i++) {
		if (mc == moder[i]) {
			answer = strjoin_cmd(cmd, " ", 2, cmd.size());
			std::cout << answer << std::endl;
			this->all_chns[fd].setTopic(answer);
			return ;
		}
	}
	answer_from_serv(mc, 482, this->file_descriptors[mc].getNickname(), cmd[1]);
	
}

void	Server::quit_cmd(std::vector<std::string> cmd, int mc) {
	std::string answer;
	std::vector<std::string> all_chns;

	answer = ":" + this->file_descriptors[mc].getNickname() + " QUIT :";
	if (cmd.size() < 2) {
		answer.append("Leaving.");
	}
	else {
		answer += strjoin_cmd(cmd, " ", 1, cmd.size());
	}
	answer.append("\n");
	all_chns = this->file_descriptors[mc].getChannels(); 
	for (size_t i = 0; i < all_chns.size(); i++){
		this->selecter_each(all_chns[i], mc, answer);
	}
	client_drop(mc);
	close(mc);
}

void	Server::kick_cmd(std::vector<std::string> cmd, int mc) {
	std::vector<std::string> chn = this->file_descriptors[mc].getChannels();
	size_t i = 0;
	std::string answer;
	std::vector<int> moder;
	std::vector<std::string> users;

	for (; i < chn.size(); i++) {
		if (cmd[1] == chn[i])
			break ;
	}
	if (i == chn.size()) {
		answer_from_serv(mc, 442, this->file_descriptors[mc].getNickname(), cmd[1]);
		return ;
	}
	int fd = this->getFdChn(cmd[1]);
	users = our_own_split(cmd[2], ",");
	moder = this->all_chns[fd].getModerator();
	for (size_t i = 0; i < moder.size(); i++) {
		if (mc == moder[i]) {
			for (size_t j = 0; j < users.size(); j++) {
				kick_name(mc, cmd, fd, users[j]);	
			}
			return ;
		}
	}
	answer_from_serv(mc, 482, this->file_descriptors[mc].getNickname(), cmd[1]);
	
}

void Server::kick_name(int mc, std::vector<std::string> cmd, int fd_chn, std::string name){
	std::vector<int> users;
	size_t i = 0;

	users = this->all_chns[fd_chn].getUsers();
	for (; i < users.size(); i++) {
		if (this->file_descriptors[users[i]].getNickname() == name) {
			this->file_descriptors[users[i]].deleteChannel(cmd[1]);
			this->all_chns[fd_chn].leaving_particimant(users[i]);
			break ;
		}
	}
	if (i == users.size()){
		return ;
	}
	std::string answer = ":" + this->file_descriptors[mc].getNickname() + "!" + this->file_descriptors[mc].getUsername() + "@" + this->host + " KICK " + cmd[1] + " " + name + " ";
	if (cmd.size() < 4)
		answer.append(":" + name);
	else
		answer += strjoin_cmd(cmd, " ", 3, cmd.size());
	answer.append("\n");
	server_answering(users[i], (char *)answer.c_str());

	users = this->all_chns[fd_chn].getClients();
	for (size_t i = 0; i < users.size(); i++) {
		server_answering(users[i], (char *)answer.c_str());
	}
}