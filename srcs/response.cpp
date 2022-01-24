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
			answer_to_client(cs, (char *)answer.append(" :None\n").c_str());
			break;
		case 315:
			answer_to_client(cs, (char *)answer.append(" :End of /WHO list\n").c_str());
			break;
		case 317:
			answer_to_client(cs, (char *)answer.append(" :second idle\n").c_str());
			break;
		case 318:
			answer_to_client(cs, (char *)answer.append(" :End of /WHOIS list\n").c_str());
			break;
		case 331:
			answer_to_client(cs, (char *)answer.append(" :No topic is set\n").c_str());
			break;
		case 366:
			answer_to_client(cs, (char *)answer.append(" :End of /NAMES list\n").c_str());
			break;
		case 401:
			answer_to_client(cs, (char *)answer.append(" :No such nick/channel\n").c_str());
			break;
		case 403:
			answer_to_client(cs, (char *)answer.append(" :No such channel\n").c_str());
			break;
		case 412:
			answer_to_client(cs, (char *)answer.append(" :No text to send\n").c_str());
			break;
		case 421:
			answer_to_client(cs, (char *)answer.append(" :Unknown command\n").c_str());
			break;
		case 432:
			answer_to_client(cs, (char *)answer.append(" :Erroneus nickname\n").c_str());
		break;
		case 433:
			answer_to_client(cs, (char *)answer.append(" :Nickname is already in use\n").c_str());
			break;
		case 442:
			answer_to_client(cs, (char *)answer.append(" :You're not on that channel\n").c_str());
			break;
		case 451:
			answer_to_client(cs, (char *)answer.append(" :You have not registered\n").c_str());
			break;
		case 461:
			answer_to_client(cs, (char *)answer.append(" :Not enough parameters\n").c_str());
			break;
		case 475:
			answer_to_client(cs, (char *)answer.append(" :Cannot join channel (+k)\n").c_str());
			break;
		case 482:
			answer_to_client(cs, (char *)answer.append(" :You're not channel operator\n").c_str());
			break;
	}
}

void	IRC::answer_to_client(int cs, char *str) {
	// std::cout << "Answer to " << this->fds[cs].get_nick() << ":\n" << str << std::endl;
	send(cs, str, strlen(str), SO_NOSIGPIPE);
}
