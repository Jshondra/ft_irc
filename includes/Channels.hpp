#pragma once
#ifndef CHANNELS_HPP
# define CHANNELS_HPP

# include <vector>
# include <iostream>

class Channels {
	private:
		std::vector<int> clients;//все клиенты
		std::vector<int> moderator;
		std::vector<int> users; //клиенты не модераторы
		std::string channel_name;
		std::string password;
		std::string topic;

		Channels(Channels *cha);
		Channels& operator=(Channels *cha);
		void	add_new_client(int mc);

	public:
		Channels();
		~Channels();

		void				clean_channel(void);
		void				add_moderator(int mc);
		void				add_users(int mc);

		std::vector<int>	getClients(void);
		std::vector<int>	getModerator(void);
		std::vector<int>	getUsers(void);

		void				setChannelName(std::string channel_name);
		std::string			getChannelName(void);

		void				setPassword(std::string password);
		std::string			getPassword(void);

		void				setTopic(std::string topic);
		std::string			getTopic(void);

		int					leaving_particimant(int mc);
};

#endif