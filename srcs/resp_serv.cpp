#include "../includes/Server.hpp"

void	Server::answer_from_serv(int mc, int num, std::string nick, std::string cmd) {
	std::string answer = ":" + this->server_name + " ";
	answer.append(std::to_string(num));
	answer.append(" ");
	answer.append(nick);
	answer.append(" ");
	answer.append(cmd);

	switch (num){
		case 303:
			server_answering(mc, (char *)answer.append(" :\n").c_str());
			break;
		case 312:
			answer.append(" "); 
			answer.append(this->server_name);
			server_answering(mc, (char *)answer.append(" \033[0;31m:None\n\033[0m").c_str());
			break;
		case 315:
			server_answering(mc, (char *)answer.append(" \033[0;31m:End of /WHO list!\n\033[0m").c_str());
			break;
		case 317:
			server_answering(mc, (char *)answer.append(" \033[0;31m:second idle!\n\033[0m").c_str());
			break;
		case 318:
			server_answering(mc, (char *)answer.append(" \033[0;31m:End of /WHOIS list!\n\033[0m").c_str());
			break;
		case 331:
			server_answering(mc, (char *)answer.append(" \033[0;31m:No topic is set!\n\033[0m").c_str());
			break;
		case 366:
			server_answering(mc, (char *)answer.append(" \033[0;31m:End of /NAMES list!\n\033[0m").c_str());
			break;
		case 401:
			server_answering(mc, (char *)answer.append(" \033[0;31m:No such nick/channel!\n\033[0m").c_str());
			break;
		case 403:
			server_answering(mc, (char *)answer.append(" \033[0;31m:No such channel!\n\033[0m").c_str());
			break;
		case 412:
			server_answering(mc, (char *)answer.append(" \033[0;31m:No text to send!\n\033[0m").c_str());
			break;
		case 421:
			server_answering(mc, (char *)answer.append(" \033[0;31m:Unknown command!\n\033[0m").c_str());
			break;
		case 432:
			server_answering(mc, (char *)answer.append(" \033[0;31m:Erroneus nickname!\n\033[0m").c_str());
		break;
		case 433:
			server_answering(mc, (char *)answer.append(" \033[0;31m:Nickname is already in use!\n\033[0m").c_str());
			break;
		case 442:
			server_answering(mc, (char *)answer.append(" \033[0;31m:You're not on that channel!\n\033[0m").c_str());
			break;
		case 451:
			server_answering(mc, (char *)answer.append(" \033[0;31m:You have not registered!\n\033[0m").c_str());
			break;
		case 461:
			server_answering(mc, (char *)answer.append(" \033[0;31m:Not enough parameters!\n\033[0m").c_str());
			break;
		case 475:
			server_answering(mc, (char *)answer.append(" \033[0;31m:Cannot join channel (+k)!\n\033[0m").c_str());
			break;
		case 482:
			server_answering(mc, (char *)answer.append(" \033[0;31m:You're not channel operator!\n\033[0m").c_str());
			break;
	}
}

void	Server::server_answering(int mc, char *str) {
	send(mc, str, strlen(str), SO_NOSIGPIPE);
}
