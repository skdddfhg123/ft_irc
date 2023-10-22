#include "../Server.hpp"

static std::string	ERR_NEEDMOREPARAMS(User user, std::string cmd) {
	return ("461 " + user.getNick() + " " + cmd + " :Not enough parameters");
}

int Server::commandPing(std::vector<std::string> args, User &user) {
	if (args.size() < 2) {
		user.reply(ERR_NEEDMOREPARAMS(user, "PING"));
		return (-1);
	}
	user.reply("PONG " + args[1]);
	return (0);
}
