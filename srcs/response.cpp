#include "../includes/IRC.hpp"

void	IRC::answer_server(int cs, int num, std::string nick, std::string cmd){
	std::string answer = ":" + this->servername + " ";
	answer.append(std::to_string(num));
	answer.append(" ");
	answer.append(nick);
	answer.append(" ");
	answer.append(cmd);

	switch (num){
		case 303:
			answer_to_client(cs, (char *)answer.append(" :\n").c_str());
			break;
		case 312:
			answer.append(" "); 
			answer.append(this->servername);
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:None\n\033[0m").c_str());
			break;
		case 315:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:End of /WHO list!\n\033[0m").c_str());
			break;
		case 317:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:second idle!\n\033[0m").c_str());
			break;
		case 318:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:End of /WHOIS list!\n\033[0m").c_str());
			break;
		case 331:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:No topic is set!\n\033[0m").c_str());
			break;
		case 366:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:End of /NAMES list!\n\033[0m").c_str());
			break;
		case 401:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:No such nick/channel!\n\033[0m").c_str());
			break;
		case 403:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:No such channel!\n\033[0m").c_str());
			break;
		case 412:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:No text to send!\n\033[0m").c_str());
			break;
		case 421:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:Unknown command!\n\033[0m").c_str());
			break;
		case 432:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:Erroneus nickname!\n\033[0m").c_str());
		break;
		case 433:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:Nickname is already in use!\n\033[0m").c_str());
			break;
		case 442:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:You're not on that channel!\n\033[0m").c_str());
			break;
		case 451:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:You have not registered!\n\033[0m").c_str());
			break;
		case 461:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:Not enough parameters!\n\033[0m").c_str());
			break;
		case 475:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:Cannot join channel (+k)!\n\033[0m").c_str());
			break;
		case 482:
			answer_to_client(cs, (char *)answer.append(" \033[0;31m:You're not channel operator!\n\033[0m").c_str());
			break;
	}
}

void	IRC::answer_to_client(int cs, char *str) {
	// std::cout << "Answer to " << this->fds[cs].get_nick_cmd() << ":!\n" << str << std::endl;
	send(cs, str, strlen(str), SO_NOSIGPIPE);
}
