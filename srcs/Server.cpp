#include "../includes/Server.hpp"

Server::Server(std::string host, int port, std::string pass) {
	maxfd = MAX_CLIENT;
	fds = new Client[maxfd];
	channels = new Channels[MAX_CHANNELS];
	this->port = port;
	this->irc_pass = pass;
	this->host = host;
	creation_server();
	all_commands();
}

Server::Server(const Server &server) {
	*this = server;
}

Server::~Server(void) {
	delete[] fds;
	delete[] channels;
}

Server&	Server::operator=(const Server &server) {
	if (this != &server){
		maxfd = server.maxfd;
		fds = server.fds;
	}
	return (*this);
}

void	Server::creation_server(void) {
	struct sockaddr_in	sin;

	this->sock = socket(PF_INET, SOCK_STREAM, 0);
	if (this->sock == -1)
	{
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit (errno);
	}
	const int trueFlag = 1;
	if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &trueFlag, sizeof(int)) < 0)
	{
		std::cout << "setsockopt failed" << std::endl;
		exit(errno);
	}
	sin.sin_family = AF_INET;
	this->servername = SERVERNAME;
	this->hostname = inet_addr((const char *)this->host.c_str());
	sin.sin_addr.s_addr = this->hostname;
	sin.sin_port = htons(this->port);
	if (bind(this->sock, (struct sockaddr*)&sin, sizeof(sin)) == -1)
	{
		std::cout << "Failed to bind to port " << port << std::endl;
		exit(errno);
	}
	if (listen(this->sock, 21) == -1)
	{
		std::cout << "Failed to listen on socket" << std::endl;
		exit(104);
	}
	fcntl(this->sock, F_SETFL, O_NONBLOCK);
	this->fds[this->sock].setFdValue(FD_SERV);
	this->fds[this->sock].setFctRead(server_accepter);
}

void	server_accepter(Server *irc, int s) {
	int					cs;
	struct sockaddr_in	csin;
	socklen_t			csin_len;

	csin_len = sizeof(csin);
	cs = accept(s, (struct sockaddr*)&csin, &csin_len);
	if (cs > irc->get_maxfd())
		close(cs);
	else {
		std::cout << "New client " << cs << " from " << inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) << std::endl;
		irc->client_adder(cs);
	}
}

void	Server::clean_read_buf(int cs) {
	fds[cs].cleanBufRead();
}

char*	Server::get_read_buf(int cs) {
	return fds[cs].getBufRead();
}

char*	Server::get_write_buf(int cs) {
	return fds[cs].getBufWrite();
}

void	writer_client(Server *irc, int cs){
	(void)cs;
	(void)irc;
}

int		Server::get_maxfd() {
	return this->maxfd;
}

int		Server::getFdValue_client(int cs){
	return this->fds[cs].getFdValue();
}

void	Server::client_adder(int cs){
	this->fds[cs].setFdValue(FD_CLIENT);
	this->fds[cs].setFctRead(reader_client);
	this->fds[cs].setFctWrite(writer_client);
}

void	Server::client_drop(int cs) {
	this->fds[cs].cleanClient();
}

void	Server::fd_make() {
	int	i;

	i = 0;
	this->max = 0;
	FD_ZERO(&this->fd_read);
	FD_ZERO(&this->fd_write);
	while (i < this->get_maxfd()){
		if (this->getFdValue_client(i) != FD_FREE){
			FD_SET(i, &this->fd_read);
			
			if (strlen(this->get_write_buf(i)) > 0)
				FD_SET(i, &this->fd_write);
			this->max = MAX(this->max, i);
			}
		i++;
	}
}

void	Server::select_to_do() {
	this->r = select(this->max + 1, &this->fd_read, &this->fd_write, nullptr, nullptr);
}

void	Server::fd_checker() {
	int	i;

	i = 0;
	while ((i < this->maxfd) && (this->r > 0)){
		if (FD_ISSET(i, &this->fd_read))
			this->reader_clienting(i);
		if (FD_ISSET(i, &this->fd_write))
			this->client_writing(i);
		if (FD_ISSET(i, &this->fd_read) || FD_ISSET(i, &this->fd_write))
			this->r--;
		i++;
	}
}

void	Server::reader_clienting(int i){
	this->fds[i].exRead(this, i);
}

void	Server::client_writing(int i){
	this->fds[i].exWrite(this, i);
}

void Server::all_commands()
{
	this->commands["NICK"] = &Server::nick_cmd;
	this->commands["USER"] = &Server::user_cmd;
	this->commands["PASS"] = &Server::pass_cmd;
	this->commands["PRIVMSG"] = &Server::privmsg_cmd;
	this->commands["NOTICE"] = &Server::ison_cmd;
	this->commands["ISON"] = &Server::ison_cmd;
	this->commands["PING"] = &Server::ping_cmd;
	this->commands["JOIN"] = &Server::join_cmd;
	this->commands["WHO"] = &Server::who_cmd;
	this->commands["WHOIS"] = &Server::whois_cmd;
	this->commands["PART"] = &Server::part_cmd;
	this->commands["AWAY"] = &Server::away_cmd;
	this->commands["NAMES"] = &Server::names_cmd;
	this->commands["TOPIC"] = &Server::topic_cmd;
	this->commands["QUIT"] = &Server::quit_cmd;
	this->commands["KICK"] = &Server::kick_cmd;
}

void	Server::command_case(std::vector<std::string> cmd, int cs)
{
	std::map<std::string, void (Server::*)(std::vector<std::string> cmd, int cs)>::iterator found;
    found = this->commands.find(cmd.front());
	if (found != commands.end())
	{
		if (cmd.size() == 1 && cmd.front() != "AWAY" && cmd.front() != "NAMES" && cmd.front() != "QUIT")
		{
			answer_server(cs, 461, this->fds[cs].getNickname(), cmd[0]);
			return ;
		}
		if (cmd[0] == "NICK" || cmd[0] == "USER" || cmd[0] == "PASS" || cmd[0] == "QUIT" || this->fds[cs].getAuth()) {
			(this->*(commands[(cmd[0])]))(cmd, cs);
		}
		else 
			answer_server(cs, 451, "", "");
	}
	else {
		if (!this->fds[cs].getAuth())
			answer_server(cs, 451, "", "");
		else
			answer_server(cs, 421, this->fds[cs].getNickname(), cmd.front());
	}
}


int		Server::get_fd(const std::string& nick) {
	for (int i = 4; i != this->get_maxfd(); ++i)
	{
		if (nick == this->fds[i].getNickname())
			return (i);
	}
	return -1;
}

bool	Server::run_checker(bool run) {
	if (run)
		return true;
	for (int fd = 3; fd < this->maxfd; ++fd)
	{
		client_drop(fd);
		close(fd);
	}
	linger opt;
	opt.l_onoff = 1;
	opt.l_linger = 0;
	setsockopt(this->sock, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt));
	close(this->sock);
	exit(130);
}

Server::Server() {
}
 