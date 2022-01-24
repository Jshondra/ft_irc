#pragma once
#ifndef CHANNELS_HPP
# define CHANNELS_HPP

# include <vector>
# include <iostream>

class Channels {
	private:
		std::vector<int> clients;
		std::vector<int> moder;
		std::vector<int> not_moder;
		std::string name_chat;
		std::string password;
		std::string topic;

		Channels(Channels *cha);
		Channels& operator=(Channels *cha);
		void	add_new_client(int cs);

	public:
		Channels(void);
		~Channels(void);

		void				clean_channel(void);
		void				add_moder(int cs);
		void				add_not_moder(int cs);
		std::vector<int>	get_users(void);
		std::vector<int>	get_list_moder(void);
		std::vector<int>	get_list_not_moder(void);
		std::string			get_name_channel(void);
		std::string			getPassword(void);
		void				setPassword(std::string password);
		void				set_name_channel(std::string name_chat);
		int					leaving_particimant(int cs);
		void				set_topic(std::string topic);
		std::string			get_topic(void);
};

#endif