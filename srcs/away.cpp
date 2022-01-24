#include "../includes/IRC.hpp"

void	IRC::away(std::vector<std::string> cmd, int cs) {
    std::string message;

    if (cmd.size() == 1)
        message = "";
    else
        message = strjoin(cmd, " ", 1, cmd.size());
    if (message.front() == ':')
		message.erase(0,1);
    this->fds[cs].set_away(message);
}
