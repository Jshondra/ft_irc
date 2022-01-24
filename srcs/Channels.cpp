#include "../includes/Channels.hpp"

Channels::Channels(void){
	this->clean_channel();
}

Channels::~Channels(void){
}

void	Channels::add_new_client(int cs){
	this->clients.push_back(cs);
}

void	Channels::add_moder(int cs){
	this->moder.push_back(cs);
	this->add_new_client(cs);
}

void	Channels::add_not_moder(int cs){
	this->not_moder.push_back(cs);
	this->add_new_client(cs);
}

int		Channels::leaving_particimant(int cs){
	for (size_t i = 0; i < this->clients.size(); i++) {
		if (this->clients[i] == cs){
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
	for (size_t i = 0; i < this->not_moder.size(); i++) {
		if (this->not_moder[i] == cs) {
			this->not_moder.erase(this->not_moder.begin() + i);
			return (0);
		}
	}
	for (size_t i = 0; i < this->moder.size(); i++) {
		if (this->moder[i] == cs) {
			this->moder.erase(this->moder.begin() + i);
			if (this->moder.empty()) {
				this->moder.push_back(this->not_moder[0]);
				this->not_moder.erase(this->not_moder.begin());
				return (this->moder[0]);
			}
		}
	}
	return -1;
}


std::vector<int>	Channels::get_users(void){
	return this->clients;
}

void	Channels::clean_channel(void){
	for (size_t i = 0; i < this->clients.size(); i++) {
		this->clients.pop_back();
	}
	for (size_t i = 0; i < this->moder.size(); i++) {
		this->moder.pop_back();
	}
	for (size_t i = 0; i < this->not_moder.size(); i++) {
		this->not_moder.pop_back();
	}
	this->name_chat = "";
	this->password = "";
	this->topic = "";
}

std::string	Channels::get_name_channel(void){
	return this->name_chat;
}

std::vector<int>	Channels::get_list_moder(void){
	return this->moder;
}

std::vector<int>	Channels::get_list_not_moder(void){
	return this->not_moder;
}

void	Channels::set_name_channel(std::string name_chat){
	this->name_chat = name_chat;
}


std::string			Channels::get_password(void){
	return this->password;
}

void				Channels::set_password(std::string password){
	this->password = password;
}

void	Channels::set_topic(std::string topic){
	this->topic = topic;
}

std::string	Channels::get_topic(void){
	return this->topic;
}
