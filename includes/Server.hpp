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
# define SERVERNAME	"Server_bro"
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
		void	ch_joiner(int cs, std::string name, std::string pass); //join_channel
		void	selecter_each(std::string cmd, int cs, std::string answer); //part_for_each
		void	method(int cs, std::string nick); //motd
		void	nickname_changer(std::string nick, int cs); //change_nick
		void	file_accepter(std::vector<std::string> cmd); //accept_file
		void	write_accepter(std::string name, int port); //accept_write
		void	kick_name(int cs, std::vector<std::string> cmd, int fd_chn, std::string name);
		
		void	user_sender(std::vector<std::string> cmd, int client_fd, int cs,\
		 std::string sender, std::string recipient); //send_user
		void	channel_sender(std::vector<std::string> cmd, int client_fd, int cs,\
		 std::string sender, std::string recipient); //send_channels

	public:
		int		r;
		fd_set	fd_read;
		fd_set	fd_write;

		Server(std::string host, int port, std::string pass);
		Server(const Server &irc);
		~Server(void);

		Server&	operator=(const Server &irc);

		void	fd_make(); //init_fd
		void	select_to_do(); //do_select
		void	fd_checker(); //check_fd
		bool	nickname_checker(const std::string& nick); //check_nick
		bool	run_checker(bool run);//check_run
		void	command_case(std::vector<std::string> cmd, int cs); //choose_cmd

		void	client_adder(int cs); //add_client
		void	clean_read_buf(int cs);
		void	client_drop(int cs); // delete_client
		bool	nickname_repl(const std::string& nick); //replay_nick

		int		get_fd_channel(std::string name);
		char	*get_read_buf(int cs);
		char	*get_write_buf(int cs);
		int		get_maxfd();
		int		get_fd(const std::string& nick);
		int		getFdValue_client(int cs);
		
		// сmd
		void	nick_cmd(std::vector<std::string> cmd, int cs); //nick
		void	user_cmd(std::vector<std::string> cmd, int cs); //user
		void	pass_cmd(std::vector<std::string> cmd, int cs); // ...
		void	privmsg_cmd(std::vector<std::string> cmd, int cs);//privmsg_notice
		void	ison_cmd(std::vector<std::string> cmd, int cs); //json
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

// void		server_accepter(Server *irc, int s); //srv_accept
// void		reader_client(Server *irc, int cs);
// void		writer_client(Server *irc, int cs);
// std::string	strjoin_cmd(std::vector<std::string> vector, const std::string& delim, int start, int end);
// std::vector<std::string>	our_own_split(const std::string& str, const std::string& sep);
// std::string	getter_clean_txt(int cs);

#endif