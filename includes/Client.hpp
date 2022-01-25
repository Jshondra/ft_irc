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
		int			fd_value;
		void		(*fct_read)(class Server *irc, int s);
		void		(*fct_write)(class Server *irc, int s);
		char		buf_read[BUF_SIZE + 1];
		char		buf_write[BUF_SIZE + 1];
		time_t		reg_time;

		std::string	nickname;
		std::string	username;
		std::string	password;
		std::string	realname;
		std::string away_message;
		std::vector<std::string> all_chns;
		bool		auth;

	public:
		Client(void);
		Client(const Client &fd);
		~Client(void);

		Client&	operator=(const Client &fd);

		void		exRead(Server *irc, int mc);
		void		exWrite(Server *irc, int mc);
		void		cleanClient(void);
		void		cleanBufRead();
		void		addChannel(std::string name);
		int			deleteChannel(std::string name);
		std::string	getChannelsStr(void);

		char		*getBufRead(void);
		char		*getBufWrite(void);
		int			getFdValue(void);
		std::string	getNickname(void);
		std::string	getUsername(void);
		std::string	getPassword(void);
		std::string	getaway_cmd(void);
		std::string	getRealname(void);
		bool		getAuth(void);
		time_t		getRegTime(void);
		std::vector<std::string> getChannels(void);
		
		void		setUsername(std::string username);
		void		setPassword(std::string password);
		void		setRealname(std::string realname);
		void		setFdValue(int);
		void		setAuth(bool auth);
		void		setNickname(std::string name);
		void		setaway_cmd(std::string message);
		void		setFctRead(void (*fct_read)(Server *irc, int s));
		void		setFctWrite(void (*fct_write)(Server *irc, int s));
		void		setRegTime(time_t reg);
};

#endif