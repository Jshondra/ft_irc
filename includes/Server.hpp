#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

# include  <iostream>
# include "Client.hpp"
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <unistd.h>
# include <vector>
# include <map>
# include "Channels.hpp"
# include <sstream>
# include <fstream>


# define MAX_CLIENT 120
# define MAX_CHANNELS 10
# define SERVERNAME	"Mickey_IRC_Mouse"
# define MAX(a,b)	((a > b) ? a : b)

class Server {
	private:
		class Client	*fds;
		class Channels	*channels;
		int				sock;
		int				port;
		int				maxfd;
		int				max;
		std::string		host;
		in_addr_t		hostname;
		std::string 	irc_pass;
		std::string		servername;
		std::map<std::string, void (Server::*)(std::vector<std::string> cmd, int cs)>	commands;
		
		Server();

		void	creation_server();
		void	all_commands();
		void	reader_clienting(int i);
		void	client_writing(int i);
		void	authorization(int cs);
		void	answer_to_client(int cs, char *str);
		void	answer_server(int cs, int num, std::string nick, std::string cmd);
		void	after_join_cmd(int cs, int chn);
		void	join_participants(int cs, int chn);
		void	ch_joiner(int cs, std::string name, std::string pass);
		void	selecter_each(std::string cmd, int cs, std::string answer);
		void	method(int cs, std::string nick);
		void	nickname_changer(std::string nick, int cs);
		void	kick_name(int cs, std::vector<std::string> cmd, int fd_chn, std::string name);
		
		void	user_sender(std::vector<std::string> cmd, int client_fd, int cs, std::string sender, std::string recipient);
		void	channel_sender(std::vector<std::string> cmd, int client_fd, int cs, std::string sender, std::string recipient);

	public:
		int		r;
		fd_set	fd_read;
		fd_set	fd_write;

		Server(std::string host, int port, std::string pass);
		Server(const Server &irc);
		~Server(void);

		Server&	operator=(const Server &irc);

		void	fd_make();
		void	select_to_do();
		void	fd_checker();
		bool	nickname_checker(const std::string& nick);
		bool	run_checker(bool run);
		void	command_case(std::vector<std::string> cmd, int cs);

		void	client_adder(int cs);
		void	clean_read_buf(int cs);
		void	client_drop(int cs);
		bool	nickname_repl(const std::string& nick);

		int		get_fd_channel(std::string name);
		char	*get_read_buf(int cs);
		char	*get_write_buf(int cs);
		int		get_maxfd();
		int		get_fd(const std::string& nick);
		int		getFdValue_client(int cs);
		
		void	nick_cmd(std::vector<std::string> cmd, int cs);
		void	user_cmd(std::vector<std::string> cmd, int cs);
		void	pass_cmd(std::vector<std::string> cmd, int cs);
		void	privmsg_cmd(std::vector<std::string> cmd, int cs);
		void	ison_cmd(std::vector<std::string> cmd, int cs);
		void	join_cmd(std::vector<std::string> cmd, int cs);
		void	ping_cmd(std::vector<std::string> cmd, int cs);
		void	who_cmd(std::vector<std::string> cmd, int cs);
		void	whois_cmd(std::vector<std::string> cmd, int cs);
		void	part_cmd(std::vector<std::string> cmd, int cs);
		void	away_cmd(std::vector<std::string> cmd, int cs);
		void	names_cmd(std::vector<std::string> cmd, int cs);
		void	topic_cmd(std::vector<std::string> cmd, int cs);
		void	quit_cmd(std::vector<std::string> cmd, int cs);
		void	kick_cmd(std::vector<std::string> cmd, int cs);
		
};

void		server_accepter(Server *irc, int s);
void		reader_client(Server *irc, int cs);
void		writer_client(Server *irc, int cs);
std::string	strjoin_cmd(std::vector<std::string> vector, const std::string& delim, int start, int end);
std::vector<std::string>	our_own_split(const std::string& str, const std::string& sep);
std::string	getter_clean_txt(int cs);

#endif