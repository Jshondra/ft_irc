#pragma once
#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include "Server.hpp"
# include <strings.h>
# include <vector>
# include <sstream>

# define FD_FREE	0
# define FD_SERV	1
# define FD_CLIENT	2
# define BUF_SIZE	4096

class Client {
	private:
		int			type;
		void		(*fct_read)(class Server *irc, int s);
		void		(*fct_write)(class Server *irc, int s);
		char		buf_read[BUF_SIZE + 1];
		char		buf_write[BUF_SIZE + 1];
		time_t		reg_time;
		std::string	nick;
		std::string	user;
		std::string	pass;
		std::string	realname;
		std::string away_mes;
		std::vector<std::string> channels;
		bool		auth;

	public:
		Client(void);
		Client(const Client &fd);
		~Client(void);

		Client&	operator=(const Client &fd);

		void		ex_read(Server *irc, int cs);
		void		ex_write(Server *irc, int cs);
		void		clean_client(void);
		void		clean_buf_read();
		void		add_channel(std::string name);
		int			delete_cannel(std::string name);
		std::string	get_channels_string(void);

		char		*get_buf_read(void);
		char		*get_buf_write(void);
		int			get_type(void);
		std::string	get_nick(void);
		std::string	get_user(void);
		std::string	get_pass(void);
		std::string	get_away(void);
		std::string	get_realname(void);
		bool		get_auth(void);
		time_t		get_reg_time(void);
		std::vector<std::string> get_channels(void);
		
		void		set_user(std::string user);
		void		set_pass(std::string pass);
		void		set_realname(std::string pass);
		void		set_type(int);
		void		set_auth(bool auth);
		void		set_nick(std::string name);
		void		set_away(std::string message);
		void		set_fct_read(void (*fct_read)(Server *irc, int s));
		void		set_fct_write(void (*fct_write)(Server *irc, int s));
		void		set_retg_time(time_t reg);
};

#endif