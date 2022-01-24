#include "../includes/Client.hpp"

Client::Client(void) {
	clean_client();
}

Client::Client(const Client &fd) {
	*this = fd;
}

Client::~Client(void) {
}

Client&	Client::operator=(const Client &fd) {
	if (this != &fd){
		type = fd.type;
		fct_read = fd.fct_read;
		fct_write = fd.fct_write;
	}
	return (*this);
}

void	Client::clean_client(void) {
	this->type = FD_FREE;
	this->fct_read = NULL;
	this->fct_write = NULL;
	this->pass = "";
	this->nick = "";
	this->user = "";
	this->auth = false;
	this->away_mes = "";
	for (size_t i = 0; i < this->channels.size(); i++) {
		this->channels.pop_back();
	}
}

void	Client::set_type(int type) {
	this->type = type;
}

void	Client::set_fct_read(void (*fct_read)(IRC *irc, int s)) {
	this->fct_read = fct_read;
}

void	Client::set_fct_write(void (*fct_write)(IRC *irc, int s)) {
	this->fct_write = fct_write;
}

void	Client::clean_buf_read() {
	bzero(buf_read, BUF_SIZE);
}

char*	Client::get_buf_read(void) {
	return buf_read;
}

char*	Client::get_buf_write(void) {
	return buf_write;
}

int		Client::get_type(void) {
	return this->type;
}

void	Client::ex_read(IRC *irc, int cs) {
	this->fct_read(irc, cs);
}
void	Client::ex_write(IRC *irc, int cs) {
	this->fct_write(irc, cs);
}

void	Client::set_nick(std::string nick) {
	this->nick = nick;
}

std::string	Client::get_nick(void) {
	return this->nick;
}

void	Client::set_user(std::string user) {
	this->user = user;
}

std::string	Client::get_user(void) {
	return this->user;
}

void Client::set_realname(std::string realname) {
	this->realname = realname;
}

std::string	Client::get_realname(void) {
	return this->realname;
}

void	Client::set_pass(std::string pass) {
	this->pass = pass;
}

std::string	Client::get_pass(void) {
	return this->pass;
}

void	Client::set_auth(bool auth) {
	this->auth = auth;
}

bool	Client::get_auth(void) {
	return this->auth;
}

void	Client::add_channel(std::string name) {
	this->channels.push_back(name);
}

int	Client::delete_cannel(std::string name) {
	for (size_t i = 0; i < this->channels.size(); i++) {
		if (this->channels[i] == name) {
			this->channels.erase(this->channels.begin() + i);
			return 0;
		}
	}
	return 1;
}

std::vector<std::string> Client::get_channels(void){
	return this->channels;
}

std::string	Client::get_channels_string(void) {
	std::string ret;

	ret = " :@";
	for (size_t i = 0; i < this->channels.size(); i++) {
		ret.append(this->channels[i]);
		if (i + 1 != this->channels.size()) {
			ret.append(" ");
		}
	}
	if (this->channels.size() == 0) {
		return (" :\n");
	}
	ret.append("\n");
	return (ret);
}

time_t		Client::get_reg_time(void) {
	return this->reg_time;
}

void		Client::set_retg_time(time_t reg){ 
	this->reg_time = reg;
}

std::string		Client::get_away(void) {
	return this->away_mes;
}

void		Client::set_away(std::string message) {
	this->away_mes = message;
}

