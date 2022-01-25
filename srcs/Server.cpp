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
