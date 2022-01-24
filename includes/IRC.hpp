#pragma once
#ifndef IRC_HPP
# define IRC_HPP

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
# define SERVERNAME	"IRC_bro"
# define MAX(a,b)	((a > b) ? a : b)

class IRC {
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
		std::map<std::string, void (IRC::*)(std::vector<std::string> cmd, int cs)>	commands;
		
		IRC();

		void	srv_create();
		void	init_cmds();
		void	client_reading(int i);
		void	client_writing(int i);
		void	authorization(int cs);
		void	answer_to_client(int cs, char *str);
		void	answer_server(int cs, int num, std::string nick, std::string cmd);
		void	after_join(int cs, int chn);
		void	join_participants(int cs, int chn);
		void	join_channel(int cs, std::string name, std::string pass);
		void	part_for_each(std::string cmd, int cs, std::string answer);
		void	motd(int cs, std::string nick);
		void	change_nick(std::string nick, int cs);
		void	accept_file(std::vector<std::string> cmd);
		void	accept_write(std::string name, int port);
		void	kick_name(int cs, std::vector<std::string> cmd, int fd_chn, std::string name);
		
		void	send_user(std::vector<std::string> cmd, int client_fd, int cs, std::string sender, std::string recipient);
		void	send_channels(std::vector<std::string> cmd, int client_fd, int cs, std::string sender, std::string recipient);

	public:
		int		r;
		fd_set	fd_read;
		fd_set	fd_write;

		IRC(std::string host, int port, std::string pass);
		IRC(const IRC &irc);
		~IRC(void);

		IRC&	operator=(const IRC &irc);

		void	init_fd();
		void	do_select();
		void	check_fd();
		bool	check_nick(const std::string& nick); 
		bool	check_run(bool run);
		void	choose_cmd(std::vector<std::string> cmd, int cs);

		void	add_client(int cs);
		void	clean_read_buf(int cs);
		void	delete_client(int cs);
		bool	replay_nick(const std::string& nick);

		int		get_fd_channel(std::string name);
		char	*get_read_buf(int cs);
		char	*get_write_buf(int cs);
		int		get_maxfd();
		int		get_fd(const std::string& nick);
		int		getFdValue_client(int cs);
		
		// сmd
		void	nick(std::vector<std::string> cmd, int cs);
		void	user(std::vector<std::string> cmd, int cs);
		void	pass(std::vector<std::string> cmd, int cs);
		void	privmsg_notice(std::vector<std::string> cmd, int cs);
		void	ison(std::vector<std::string> cmd, int cs);
		void	join(std::vector<std::string> cmd, int cs);
		void	ping(std::vector<std::string> cmd, int cs);
		void	who(std::vector<std::string> cmd, int cs);
		void	whois(std::vector<std::string> cmd, int cs);
		void	part(std::vector<std::string> cmd, int cs);
		void	away(std::vector<std::string> cmd, int cs);
		void	names(std::vector<std::string> cmd, int cs);
		void	topic(std::vector<std::string> cmd, int cs);
		void	quit(std::vector<std::string> cmd, int cs);
		void	kick(std::vector<std::string> cmd, int cs);
		
};

void		srv_accept(IRC *irc, int s);
void		client_read(IRC *irc, int cs);
void		client_write(IRC *irc, int cs);
std::string	strjoin(std::vector<std::string> vector, const std::string& delim, int start, int end);
std::vector<std::string>	split(const std::string& str, const std::string& sep);
std::string	get_text(int cs);

#endif