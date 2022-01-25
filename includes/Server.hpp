#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

# include  <iostream>
# include <fcntl.h>
# include <fstream>
# include <map>
# include "Client.hpp"
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
# include "Channels.hpp"
# include <sstream>
# include <unistd.h>
# include <vector>


# define MAX_CHANNELS 10
# define MAX_CLIENT 120
# define SERVERNAME	"Mickey_IRC_Mouse"
# define MAX(a,b)	((a > b) ? a : b)

class Server {
	private:
		class Channels	*all_chns;
		class Client	*file_descriptors;
		int				fd_maximum;
		int				maximum;
		int				sockets;
		int				port;
		std::string		host;
		in_addr_t		hostname;
		std::string 	password_server;
		std::string		server_name;
		std::map<std::string, void (Server::*)\
			(std::vector<std::string> cmd, int mc)>	allCommands;
		
		Server();

		void	creation_server();
		void	all_commands();
		void	clientIsReading(int i);
		void	clientIsWriting(int i);
		void	make_auth(int mc);
		void	after_join_cmd(int mc, int chn);
		void	method(int mc, std::string nick);
		void	users_join_to_chn(int mc, int chn);
		void	server_answering(int mc, char *str);
		void	nickname_changer(std::string nick, int mc);
		void	ch_joiner(int mc, std::string name, std::string pass);
		void	selecter_each(std::string cmd, int mc, std::string answer);
		void	answer_from_serv(int mc, int num, std::string nick, std::string cmd);
		void	kick_name(int mc, std::vector<std::string> cmd,\
				int fd_chn, std::string name);
		void	channel_sender(std::vector<std::string> cmd, int client_fd,\
		 int mc, std::string sender, std::string recipient);
		void	user_sender(std::vector<std::string> cmd, int client_fd, int mc,\
				std::string sender, std::string recipient);

	public:
		int		rack;
		fd_set	writeFd;
		fd_set	readFd;
		
		Server(const Server &irc);
		~Server(void);
		Server(std::string host, int port, std::string pass);
		Server&	operator=(const Server &irc);

		void	fd_make();
		void	select_to_do();
		void	fd_checker();
		bool	nickname_checker(const std::string& nick);
		bool	run_checker(bool run);
		void	command_case(std::vector<std::string> cmd, int mc);

		void	client_adder(int mc);
		void	readCleanBuffer(int mc);
		void	client_drop(int mc);
		bool	nickname_repl(const std::string& nick);

		int		getFdMax();
		char	*getBufferRead(int mc);
		char	*getBufferWrite(int mc);
		int		getFdChn(std::string name);
		int		getFd(const std::string& nick);
		int		getFdValue_client(int mc);
		
		void	who_cmd(std::vector<std::string> cmd, int mc);
		void	whois_cmd(std::vector<std::string> cmd, int mc);
		void	part_cmd(std::vector<std::string> cmd, int mc);
		void	away_cmd(std::vector<std::string> cmd, int mc);
		void	names_cmd(std::vector<std::string> cmd, int mc);
		void	topic_cmd(std::vector<std::string> cmd, int mc);
		void	quit_cmd(std::vector<std::string> cmd, int mc);
		void	kick_cmd(std::vector<std::string> cmd, int mc);
		void	nick_cmd(std::vector<std::string> cmd, int mc);
		void	user_cmd(std::vector<std::string> cmd, int mc);
		void	pass_cmd(std::vector<std::string> cmd, int mc);
		void	privmsg_cmd(std::vector<std::string> cmd, int mc);
		void	ison_cmd(std::vector<std::string> cmd, int mc);
		void	join_cmd(std::vector<std::string> cmd, int mc);
		void	ping_cmd(std::vector<std::string> cmd, int mc);
		
		
};

void		reader_client(Server *irc, int mc);
void		writer_client(Server *irc, int mc);
void		server_accepter(Server *irc, int s);

std::string	getter_clean_txt(int mc);
std::vector<std::string>	our_own_split(const std::string& str,\
	 const std::string& sep);
std::string	strjoin_cmd(std::vector<std::string> vector,\
	 const std::string& delim, int start, int end);

#endif