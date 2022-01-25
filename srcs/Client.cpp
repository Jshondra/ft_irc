#include "../includes/Client.hpp"

Client::Client(void) {
	cleanClient();
}

Client::Client(const Client &fd) {
	*this = fd;
}

Client::~Client(void) {
}

Client&	Client::operator=(const Client &fd) {
	if (this != &fd){
		fd_value = fd.fd_value;
		fct_read = fd.fct_read;
		fct_write = fd.fct_write;
	}
	return (*this);
}

void	Client::cleanClient(void) {
	this->fd_value = FD_FREE;
	this->fct_read = NULL;
	this->fct_write = NULL;
	this->password = "";
	this->nickname = "";
	this->username = "";
	this->auth = false;
	this->away_message = "";
	for (size_t i = 0; i < this->channels.size(); i++) {
		this->channels.pop_back();
	}
}

void	Client::setFdValue(int fd_value) {
	this->fd_value = fd_value;
}

void	Client::setFctRead(void (*fct_read)(Server *irc, int s)) {
	this->fct_read = fct_read;
}

void	Client::setFctWrite(void (*fct_write)(Server *irc, int s)) {
	this->fct_write = fct_write;
}

void	Client::cleanBufRead() {
	bzero(buf_read, BUF_SIZE);
}

char*	Client::getBufRead(void) {
	return buf_read;
}

char*	Client::getBufWrite(void) {
	return buf_write;
}

int		Client::getFdValue(void) {
	return this->fd_value;
}

void	Client::exRead(Server *irc, int cs) {
	this->fct_read(irc, cs);
}
void	Client::exWrite(Server *irc, int cs) {
	this->fct_write(irc, cs);
}

void	Client::setNickname(std::string nickname) {
	this->nickname = nickname;
}

std::string	Client::getNickname(void) {
	return this->nickname;
}

void	Client::setUsername(std::string username) {
	this->username = username;
}

std::string	Client::getUsername(void) {
	return this->username;
}

void Client::setRealname(std::string realname) {
	this->realname = realname;
}

std::string	Client::getRealname(void) {
	return this->realname;
}

void	Client::setPassword(std::string password) {
	this->password = password;
}

std::string	Client::getPassword(void) {
	return this->password;
}

void	Client::setAuth(bool auth) {
	this->auth = auth;
}

bool	Client::getAuth(void) {
	return this->auth;
}

void	Client::addChannel(std::string name) {
	this->channels.push_back(name);
}

int	Client::deleteChannel(std::string name) {
	for (size_t i = 0; i < this->channels.size(); i++) {
		if (this->channels[i] == name) {
			this->channels.erase(this->channels.begin() + i);
			return 0;
		}
	}
	return 1;
}

std::vector<std::string> Client::getChannels(void){
	return this->channels;
}

std::string	Client::getChannelsStr(void) {
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

time_t		Client::getRegTime(void) {
	return this->reg_time;
}

void		Client::setRegTime(time_t reg){ 
	this->reg_time = reg;
}

std::string		Client::getaway_cmd(void) {
	return this->away_message;
}

void		Client::setaway_cmd(std::string message) {
	this->away_message = message;
}

