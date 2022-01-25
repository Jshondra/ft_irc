#include "../includes/IRC.hpp"

# define OFF_COLOR "\033[0m"
# define RED "\033[0;31m"
# define GREEN "\033[0;32m"
# define YELLOW "\033[0;33"
# define BLUE "\033[0;34"
# define VIOLET "\033[0;35"
# define LBLUE "\033[0;36"
# define GREY "\033[0;37"

IRC::IRC(std::string host, int port, std::string pass) {
	maxfd = MAX_CLIENT;
	fds = new Client[maxfd];
	channels = new Channels[MAX_CHANNELS];
	this->port = port;
	this->irc_pass = pass;
	this->host = host;
	srv_create();
	init_cmds();
}

IRC::IRC(const IRC &irc) {
	*this = irc;
}

IRC::~IRC(void) {
	delete[] fds;
	delete[] channels;
}

IRC&	IRC::operator=(const IRC &irc) {
	if (this != &irc){
		maxfd = irc.maxfd;
		fds = irc.fds;
	}
	return (*this);
}

void	IRC::srv_create(void) {
	struct sockaddr_in	sin;

	this->sock = socket(PF_INET, SOCK_STREAM, 0);
	if (this->sock == -1) {
		std::cout << "\033[0;31mFailed to create socket. errno: " << errno << OFF_COLOR << std::endl;
		exit (errno);
	}
	const int trueFlag = 1;
	if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &trueFlag, sizeof(int)) < 0) {
		std::cout << "\033[0;31msetsockopt failed" << OFF_COLOR << std::endl;
		exit(errno);
	}
	sin.sin_family = AF_INET;
	this->servername = SERVERNAME;
	this->hostname = inet_addr((const char *)this->host.c_str());
	sin.sin_addr.s_addr = this->hostname;
	sin.sin_port = htons(this->port);
	if (bind(this->sock, (struct sockaddr*)&sin, sizeof(sin)) == -1) {
		std::cout << "\033[0;31mFailed to bind to port " << port << OFF_COLOR << std::endl;
		exit(errno);
	}
	if (listen(this->sock, 21) == -1) {
		std::cout << "\033[0;31mFailed to listen on socket \033[0m" << std::endl;
		exit(104);
	}
	fcntl(this->sock, F_SETFL, O_NONBLOCK);
	this->fds[this->sock].setFdValue(FD_SERV);
	this->fds[this->sock].setFctRead(server_accepter);
}

void	server_accepter(IRC *irc, int s) {
	int					cs;
	struct sockaddr_in	csin;
	socklen_t			csin_len;

	csin_len = sizeof(csin);
	cs = accept(s, (struct sockaddr*)&csin, &csin_len);
	if (cs > irc->get_maxfd())
		close(cs);
	else {
		std::cout << "\033[0;32mNew client " << cs << " from " << inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) << " \033[0m" << std::endl;
		irc->client_adder(cs);
	}
}

void	IRC::clean_read_buf(int cs) {
	fds[cs].cleanBufRead();
}

char*	IRC::get_read_buf(int cs) {
	return fds[cs].getBufRead();
}

char*	IRC::get_write_buf(int cs) {
	return fds[cs].getBufWrite();
}

void	writer_client(IRC *irc, int cs){
	(void)cs;
	(void)irc;
}

int		IRC::get_maxfd() {
	return this->maxfd;
}

int		IRC::getFdValue_client(int cs){
	return this->fds[cs].getFdValue();
}

void	IRC::client_adder(int cs){
	this->fds[cs].setFdValue(FD_CLIENT);
	this->fds[cs].setFctRead(reader_client);
	this->fds[cs].setFctWrite(writer_client);
}

void	IRC::client_drop(int cs) {
	this->fds[cs].cleanClient();
}

void	IRC::fd_make() {
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

void	IRC::select_to_do() {
	this->r = select(this->max + 1, &this->fd_read, &this->fd_write, nullptr, nullptr);
}

void	IRC::fd_checker() {
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

void	IRC::reader_clienting(int i){
	this->fds[i].exRead(this, i);
}

void	IRC::client_writing(int i){
	this->fds[i].exWrite(this, i);
}

void IRC::init_cmds()
{
	this->commands["NICK"] = &IRC::nick_cmd;
	this->commands["USER"] = &IRC::user_cmd;
	this->commands["PASS"] = &IRC::pass_cmd;
	this->commands["PRIVMSG"] = &IRC::privmsg_cmd;
	this->commands["NOTICE"] = &IRC::ison_cmd;
	this->commands["ISON"] = &IRC::ison_cmd;
	this->commands["PING"] = &IRC::ping_cmd;
	this->commands["JOIN"] = &IRC::join_cmd;
	this->commands["WHO"] = &IRC::who_cmd;
	this->commands["WHOIS"] = &IRC::whois_cmd;
	this->commands["PART"] = &IRC::part_cmd;
	this->commands["AWAY"] = &IRC::away_cmd;
	this->commands["NAMES"] = &IRC::names_cmd;
	this->commands["TOPIC"] = &IRC::topic_cmd;
	this->commands["QUIT"] = &IRC::quit_cmd;
	this->commands["KICK"] = &IRC::kick_cmd;
}

void	IRC::command_case(std::vector<std::string> cmd, int cs)
{
	std::map<std::string, void (IRC::*)(std::vector<std::string> cmd, int cs)>::iterator found;
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


int		IRC::get_fd(const std::string& nick) {
	for (int i = 4; i != this->get_maxfd(); ++i)
	{
		if (nick == this->fds[i].getNickname())
			return (i);
	}
	return -1;
}

bool	IRC::run_checker(bool run) {
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

IRC::IRC() {}
 