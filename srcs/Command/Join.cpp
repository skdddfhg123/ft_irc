#include "../Server.hpp"

static bool	isUserinChannel(Channel &channel, int fd) {
	for (unsigned int i = 0; i < channel.getUsers().size(); i++) {
		if (channel.getUsers()[i].getFd() == fd)
			return (true);
	}
	return (false);
}

static std::string	RPL_TOPIC(User user, std::string channel, std::string topic) {
	return ("332 " + user.getNick() + " " + channel + " " + topic);
}

static std::string	RPL_NAMREPLY(User &user, std::string channelName, std::string users) {
	std::string ret = "353 " + user.getNick() + " = " + channelName + " :" + users;
	std::cout << "namereply " << ret << std::endl;
	return (ret);
}

static std::string	RPL_ENDOFNAMES(User &user, std::string channelName) {
	return ("366 " + user.getNick() + " " + channelName + " :End of /NAMES list.");
}

static std::string	ERR_NEEDMOREPARAMS(User user, std::string cmd) {
	return ("461 " + user.getNick() + " " + cmd + " :Not enough parameters");
}

static std::string	ERR_CHANNELISFULL(User user, std::string name) {
	return ("471 " + user.getNick() + " " + name + " :Cannot join channel(full)");
}

static std::string	ERR_BADCHANNELKEY(User user, std::string name) {
	return ("475 " + user.getNick() + " " + name + " :bad channel mask");
}

static void	join(Channel &channel, User &user) {
	std::string	users;
	for(unsigned int i = 0; i < channel.getUsers().size(); i++) {
		if (channel.getUsers()[i].getFd() == channel.getFdAuth())
			users += "@" + channel.getUsers()[i].getNick() + " ";
		else
			users += channel.getUsers()[i].getNick() + " ";
	}
	channel.broadcast(user.getPrefix() + " JOIN :" + channel.getName());
	user.reply(RPL_TOPIC(user, channel.getName(), channel.getTopic()));
	user.reply(RPL_NAMREPLY(user, channel.getName(), users));
	user.reply(RPL_ENDOFNAMES(user, channel.getName()));
}

int	Server::commandJoin(std::vector<std::string> args, User &user) {
	if (user.getState() != REGISTERED) {
		user.reply("Registration must be completed first");
		return (-1);
	}
	if (args.size() < 2) {
		user.reply(ERR_NEEDMOREPARAMS(user, "JOIN"));
		return (-1);
	}
	std::string name = deleteR(args[1]);
	std::cout << "name = " << name<< std::endl;
	if (!name[0] || name[0] != '#') {
		user.reply("Error : Channel must start with '#'");
		return (-1);
	}
	try {
		std::vector<Channel>::iterator channel = findChannelIt(name);

		if (isUserinChannel(*channel, user.getFd())) {
			std::cout << "is already in channel" << std::endl;
			return (-1);
		}
		if (findChannel(name).getUsers().size() >= findChannel(name).getMaxUser() 
		&& findChannel(name).getMaxUser() != 0) {
			user.reply(ERR_CHANNELISFULL(user, name));
			return (-1);
		}
		if (findChannel(name).getPassword() != "" && args.size() == 3) {
			if (deleteR(args.at(2)) != findChannel(name).getPassword()) {
				user.reply("Invalid Password");
				return (-1);
			}
		}
		if (findChannel(name).getPassword() != "" && args.size() < 3)
			user.reply(ERR_BADCHANNELKEY(user, name));
		channel->addUser(user);
		join(*channel, user);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		Channel newChannel(name);
		newChannel.addUser(user);
		newChannel.setFdAuth(user.getFd());
		_channels.push_back(newChannel);
		join(newChannel, user);
	}
	return (0);
}
