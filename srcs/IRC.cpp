#include "../includes/IRC.hpp"

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
	this->fds[this->sock].set_type(FD_SERV);
	this->fds[this->sock].set_fct_read(srv_accept);
}

void	srv_accept(IRC *irc, int s) {
	int					cs;
	struct sockaddr_in	csin;
	socklen_t			csin_len;

	csin_len = sizeof(csin);
	cs = accept(s, (struct sockaddr*)&csin, &csin_len);
	if (cs > irc->get_maxfd())
		close(cs);
	else {
		std::cout << "New client " << cs << " from " << inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) << std::endl;
		irc->add_client(cs);
	}
}

void	IRC::clean_read_buf(int cs) {
	fds[cs].clean_buf_read();
}

char*	IRC::get_read_buf(int cs) {
	return fds[cs].get_buf_read();
}

char*	IRC::get_write_buf(int cs) {
	return fds[cs].get_buf_write();
}

void	client_write(IRC *irc, int cs){
	(void)cs;
	(void)irc;
}

int		IRC::get_maxfd() {
	return this->maxfd;
}

int		IRC::get_type_client(int cs){
	return this->fds[cs].get_type();
}

void	IRC::add_client(int cs){
	this->fds[cs].set_type(FD_CLIENT);
	this->fds[cs].set_fct_read(client_read);
	this->fds[cs].set_fct_write(client_write);
}

void	IRC::delete_client(int cs) {
	this->fds[cs].clean_client();
}

void	IRC::init_fd() {
	int	i;

	i = 0;
	this->max = 0;
	FD_ZERO(&this->fd_read);
	FD_ZERO(&this->fd_write);
	while (i < this->get_maxfd()){
		if (this->get_type_client(i) != FD_FREE){
			FD_SET(i, &this->fd_read);
			
			if (strlen(this->get_write_buf(i)) > 0)
				FD_SET(i, &this->fd_write);
			this->max = MAX(this->max, i);
			}
		i++;
	}
}

void	IRC::do_select() {
	this->r = select(this->max + 1, &this->fd_read, &this->fd_write, nullptr, nullptr);
}

void	IRC::check_fd() {
	int	i;

	i = 0;
	while ((i < this->maxfd) && (this->r > 0)){
		if (FD_ISSET(i, &this->fd_read))
			this->client_reading(i);
		if (FD_ISSET(i, &this->fd_write))
			this->client_writing(i);
		if (FD_ISSET(i, &this->fd_read) || FD_ISSET(i, &this->fd_write))
			this->r--;
		i++;
	}
}

void	IRC::client_reading(int i){
	this->fds[i].ex_read(this, i);
}

void	IRC::client_writing(int i){
	this->fds[i].ex_write(this, i);
}

void IRC::init_cmds()
{
	this->commands["NICK"] = &IRC::nick;
	this->commands["USER"] = &IRC::user;
	this->commands["PASS"] = &IRC::pass;
	this->commands["PRIVMSG"] = &IRC::privmsg_notice;
	this->commands["NOTICE"] = &IRC::privmsg_notice;
	this->commands["ISON"] = &IRC::ison;
	this->commands["PING"] = &IRC::ping;
	this->commands["JOIN"] = &IRC::join;
	this->commands["WHO"] = &IRC::who;
	this->commands["WHOIS"] = &IRC::whois;
	this->commands["PART"] = &IRC::part;
	this->commands["AWAY"] = &IRC::away;
	this->commands["NAMES"] = &IRC::names;
	this->commands["TOPIC"] = &IRC::topic;
	this->commands["QUIT"] = &IRC::quit;
	this->commands["KICK"] = &IRC::kick;
}

void	IRC::choose_cmd(std::vector<std::string> cmd, int cs)
{
	std::map<std::string, void (IRC::*)(std::vector<std::string> cmd, int cs)>::iterator found;
    found = this->commands.find(cmd.front());
	if (found != commands.end())
	{
		if (cmd.size() == 1 && cmd.front() != "AWAY" && cmd.front() != "NAMES" && cmd.front() != "QUIT")
		{
			answer_server(cs, 461, this->fds[cs].get_nick(), cmd[0]);
			return ;
		}
		if (cmd[0] == "NICK" || cmd[0] == "USER" || cmd[0] == "PASS" || cmd[0] == "QUIT" || this->fds[cs].get_auth()) {
			(this->*(commands[(cmd[0])]))(cmd, cs);
		}
		else 
			answer_server(cs, 451, "", "");
	}
	else {
		if (!this->fds[cs].get_auth())
			answer_server(cs, 451, "", "");
		else
			answer_server(cs, 421, this->fds[cs].get_nick(), cmd.front());
	}
}


int		IRC::get_fd(const std::string& nick) {
	for (int i = 4; i != this->get_maxfd(); ++i)
	{
		if (nick == this->fds[i].get_nick())
			return (i);
	}
	return -1;
}

bool	IRC::check_run(bool run) {
	if (run)
		return true;
	for (int fd = 3; fd < this->maxfd; ++fd)
	{
		delete_client(fd);
		close(fd);
	}
	linger opt;
	opt.l_onoff = 1;
	opt.l_linger = 0;
	setsockopt(this->sock, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt));
	close(this->sock);
	exit(130);
}

IRC::IRC() {
}
 