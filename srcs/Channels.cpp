#include "../includes/Channels.hpp"

Channels::Channels(){
	this->clean_channel();
}

Channels::~Channels(){}

void	Channels::add_new_client(int mc){
	this->clients.push_back(mc);
}

void	Channels::clean_channel(void){
	for (size_t i = 0; i < this->clients.size(); i++) {
		this->clients.pop_back();
	}
	for (size_t i = 0; i < this->moderator.size(); i++) {
		this->moderator.pop_back();
	}
	for (size_t i = 0; i < this->users.size(); i++) {
		this->users.pop_back();
	}
	this->channel_name = "";
	this->password = "";
	this->topic = "";
}

void	Channels::add_moderator(int mc){
	this->moderator.push_back(mc);
	this->add_new_client(mc);
}

void	Channels::add_users(int mc){
	this->users.push_back(mc);
	this->add_new_client(mc);
}

std::vector<int>	Channels::getClients(void){
	return this->clients;
}

std::vector<int>	Channels::getModerator(void){
	return this->moderator;
}

std::vector<int>	Channels::getUsers(void){
	return this->users;
}

void	Channels::setChannelName(std::string channel_name){
	this->channel_name = channel_name;
}

std::string	Channels::getChannelName(void){
	return this->channel_name;
}

void				Channels::setPassword(std::string password){
	this->password = password;
}

std::string			Channels::getPassword(void){
	return this->password;
}

void	Channels::setTopic(std::string topic){
	this->topic = topic;
}

std::string	Channels::getTopic(void){
	return this->topic;
}

int		Channels::leaving_particimant(int mc){
	for (size_t i = 0; i < this->clients.size(); i++) {
		if (this->clients[i] == mc){
			this->clients.erase(this->clients.begin() + i);
			break ;
		}
		if (i == this->clients.size()) {
			return -1;
		}
	}
	if (this->clients.empty()){
		this->clean_channel();
		return -1;
	}
	for (size_t i = 0; i < this->users.size(); i++) {
		if (this->users[i] == mc) {
			this->users.erase(this->users.begin() + i);
			return (0);
		}
	}
	for (size_t i = 0; i < this->moderator.size(); i++) {
		if (this->moderator[i] == mc) {
			this->moderator.erase(this->moderator.begin() + i);
			if (this->moderator.empty()) {
				this->moderator.push_back(this->users[0]);
				this->users.erase(this->users.begin());
				return (this->moderator[0]);
			}
		}
	}
	return -1;
}