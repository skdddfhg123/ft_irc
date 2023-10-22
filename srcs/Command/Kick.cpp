#include "../Server.hpp"

static bool	isUserinChannel(Channel &channel, int fd) {
	for (unsigned int i = 0; i < channel.getUsers().size(); i++) {
		if (channel.getUsers()[i].getFd() == fd)
			return (true);
	}
	return (false);
}

static bool	isUserNInChannel(Channel &channel, std::string name) {
	for (unsigned int i = 0; i < channel.getUsers().size(); i++) {
		if (channel.getUsers()[i].getNick() == name)
			return (true);
	}
	return (false);
}

static std::string	ERR_NOSUCHCHANNEL(User user, std::string channelName) {
	return ("403 " + user.getNick() + " " + channelName + " :No such channel");
}

static std::string	ERR_USERNOTINCHANNEL(User user, std::string nickName, std::string channelName) {
	return ("441 " + user.getNick() + " " + nickName + " " +channelName + " :They aren't on that channel");
}

static std::string	ERR_NOTONCHANNEL(User user, std::string channelName) {
	return ("442 " + user.getNick() + " " + channelName + " :You're not on that channel");
}

static std::string	ERR_NEEDMOREPARAMS(User user, std::string cmd) {
	return ("461 " + user.getNick() + " " + cmd + " :Not enough parameters");
}

static std::string	ERR_CHANOPRIVSNEEDED(User user, std::string channelName) {
	return ("482 " + user.getNick() + " " + channelName + " :You're not channel operator");
}

int	Server::commandKick(std::vector<std::string> args, User &user) {
	std::string	cmd = args.at(0);
	if (args.size() < 3) {
		user.reply(ERR_NEEDMOREPARAMS(user, "KICK"));
		return (-1);
	}
	for (unsigned int i = 0; i < _channels.size(); i++) {
		if (_channels[i].getName() != args.at(1)) {
			user.reply(ERR_NOSUCHCHANNEL(user, args.at(1)));
			return (-1);
		}
	}
	if (isUserinChannel(findChannel(args.at(1)), user.getFd()) == false) {
		user.reply(ERR_NOTONCHANNEL(user, args.at(1)));
		return (-1);
	}
	if (user.getFd() != findChannel(args.at(1)).getFdAuth()) {
		user.reply(ERR_CHANOPRIVSNEEDED(user, args.at(1)));
		return (-1);
	}
	if (isUserNInChannel(findChannel(args.at(1)), deleteR(args.at(2))) == false) {
		user.reply(ERR_USERNOTINCHANNEL(user, deleteR(args.at(2)), args.at(1)));
		return (-1);
	}
	std::vector<std::string> tmp;
	tmp.push_back("PART");
	tmp.push_back(args.at(1));
	commandPart(tmp, findUser(args.at(2)));
	return (0);
}
