#include "../includes/IRC.hpp"
#include "../includes/Client.hpp"

# define OFF_COLOR "\033[0m"
# define RED "\033[0;31m"
# define GREEN "\033[0;32m"
# define YELLOW "\033[0;33"
# define BLUE "\033[0;34"
# define VIOLET "\033[0;35"
# define LBLUE "\033[0;36"
# define GREY "\033[0;37"


bool	g_run;

void	handler(int sig)
{
	(void)sig;
	g_run = false;
}

void start_server(std::string host, int port, std::string pass)
{
	IRC irc(host, port, pass);
	
	g_run = true;
	signal(SIGINT, handler);
	while (irc.run_checker(g_run))
	{
		irc.fd_make();
		irc.select_to_do();
		irc.fd_checker();
	}
}

int main(int argc, char **argv){
	std::string pass;
	std::string host;
	int			port;

	if (argc == 2) {
		std::vector<std::string> serv_conf;
		std::stringstream ss(argv[1]);
		std::string buf;

		while (std::getline(ss, buf, ':'))
			if (!buf.empty())
				serv_conf.push_back(buf);

		if (serv_conf.size() != 3) {
			std::cout<< "\033[0;31musage: ircserv [host:port:password] <port> <password>\033[0m" << std::endl;
			exit(1);
		}
		host = serv_conf[0];
		port = atoi(serv_conf[1].c_str());
		pass = serv_conf[2];
	}
	else if (argc == 3)
	{
		host = "127.0.0.1";
		port = atoi(argv[1]);
		pass = argv[2];
	}
	else {
		std::cout << "\033[0;31musage: ircserv [host:port:password] <port> <password>\033[0m" << std::endl;
		exit (1);
	}
	if (port < 1024 || port > 49151)
	{
		std::cout << "\033[0;31mWrong port!!!\033[0m" << std::endl;
		exit (1);
	}
	start_server(host, port, pass);
	return (0);
}
