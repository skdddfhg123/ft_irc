#include "../Server.hpp"

static bool	isUserinChannel(Channel &channel, int fd)
{
	for (unsigned int i = 0; i < channel.getUsers().size(); i++) {
		if (channel.getUsers()[i].getFd() == fd)
			return (true);
	}
	return (false);
}

std::string RPL_NOTICE(User user, std::string recipient, std::string msg) {
	return (":" + user.getNick() + " NOTICE " + recipient + " :" + msg);
}

static std::string	ERR_NEEDMOREPARAMS(User user, std::string cmd) {
	return ("461 " + user.getNick() + " " + cmd + " :Not enough parameters");
}

static std::string	ERR_CANNOTSENDTOCHAN(User user, Channel &channel) {
	return ("404 " + user.getNick() + " you are not in the channel " + channel.getName());
}

int Server::commandNotice(std::vector<std::string> args, User &user) {
	std::cout << "!!!!!!NOTICE!!!!!" << std::endl;
	if (user.getState() != REGISTERED) {
		user.reply("You have to register first.");
		return (-1);
	}
	if (args.size() < 3) {
		user.reply(ERR_NEEDMOREPARAMS(user, "NOTICE"));
		return (-1);
	}
	if (args[1].at(0) == '#') {
		if (args.size() < 3) {
			user.reply(ERR_NEEDMOREPARAMS(user, "NOTICE"));
			return (-1);
		}
		std::string msg = getMessage(args);
		try {
			std::vector<Channel>::iterator channel = findChannelIt(args[1]);
			if (isUserinChannel(*channel, user.getFd()))
				channel->broadcast(RPL_NOTICE(user, args[1], msg), user);
			else
				user.reply(ERR_CANNOTSENDTOCHAN(user, *channel));
		}
		catch (const std::exception& e) {
			user.reply(e.what());
		}
		return (0);
	}
	try {
		User		recipient = findUser(args[1]);      
		std::string	msg = getMessage(args);
		std::cout << "message = " << "[" << msg << "]" << std::endl;
		std::string package = RPL_NOTICE(user, recipient.getNick(), msg);
		std::cout << package << recipient.getFd() << std::endl;
		package += "\r\n";
		if (send(recipient.getFd(), package.c_str(), package.length(), 0) < 0)
			throw std::out_of_range("error while sendig in privmsg");
	}
	catch(const std::exception& e) {
		user.reply(e.what());
		std::cerr << e.what() << '\n';
	}

	return (0);
}
