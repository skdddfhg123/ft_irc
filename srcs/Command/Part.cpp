#include "../Server.hpp"

static std::string	ERR_NEEDMOREPARAMS(User user, std::string cmd) {
	return ("461 " + user.getNick() + " " + cmd + " :Not enough parameters");
}

static std::string	ERR_NOSUCHCHANNEL(User user, std::string channelName) {
	return ("403 " + user.getNick() + " " + channelName + " :No such channel");
}

int Server::commandPart(std::vector<std::string> args, User &user) {
	if (user.getState() != REGISTERED) {
		user.reply("You have to register first.");
		return (-1);
	}
	if (args.size() < 2) {
		user.reply(ERR_NEEDMOREPARAMS(user, "PART"));
		return (-1);
	}
	try {
		std::string channelName = deleteR(args[1]);
		std::vector<Channel>::iterator channel = findChannelIt(channelName);
		if (channel->getFdAuth() == user.getFd())
			channel->setFdAuth(0);
		channel->deleteUser(user);
		if (channel->getUsers().empty())
			_channels.erase(channel);
	}
	catch(const std::exception& e) {
		user.reply(ERR_NOSUCHCHANNEL(user, "PART"));
	}

	return (0);
}
