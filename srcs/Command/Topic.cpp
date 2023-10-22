#include "../Server.hpp"

static bool	isUserinChannel(Channel &channel, int fd) {
	for (unsigned int i = 0; i < channel.getUsers().size(); i++) {
		if (channel.getUsers()[i].getFd() == fd)
			return (true);
	}
	return (false);
}

static bool	isOperInChannel(User user, Channel channel) {
	if (channel.getFdAuth() == user.getFd())
		return (true);
	return (false);
}

static std::string	deleteR(std::string str) {
	if (str.empty())
		return "";
	if (str[str.size() - 1] == '\r')
		return str.substr(0, (str.size() - 1));
	return str;
}

static std::string	topicStr(std::vector<std::string> args)
{
	std::string topic;

	unsigned int i = 3;
	topic = args[2];
	if (topic[0] == ':')
		topic = topic.substr(1);
	while (i < args.size()) {
		topic += " ";
		topic += args[i];
		i++;
	}
	topic = deleteR(topic);
	return (topic);
}

static std::string  RPL_NOTOPIC(User user, std::string channel) {
	return ("331 " + user.getNick() + " " + channel +" :No topic is set");
}

static std::string  RPL_TOPIC(User user, std::string channel, std::string topic) {
	return ("332 " + user.getNick() + " " + channel + " " + topic);
}

static std::string	ERR_NOSUCHCHANNEL(User user, std::string channelName) {
	return ("403 " + user.getNick() + " " + channelName + " :No such channel");
}

static std::string  ERR_NOTONCHANNEL(User user, std::string channel) {
	return ("442 " + user.getNick() + " " + channel + " :You're not on that channel");
}

static std::string	ERR_NEEDMOREPARAMS(User user, std::string cmd) {
	return ("461 " + user.getNick() + " " + cmd + " :Not enough parameters");
}

static std::string	ERR_CHANOPRIVSNEEDED(User user, std::string channelName) {
	return ("482 " + user.getNick() + " " + channelName + " :You're not channel operator");
}

int		Server::commandTopic(std::vector<std::string> args, User &user) {
	if (args.size() < 2) {
		user.reply(ERR_NEEDMOREPARAMS(user, "TOPIC"));
		return (-1);
	}
	std::string channelName = deleteR(args[1]);
	if (channelName.empty()) {
		user.reply(ERR_NEEDMOREPARAMS(user, "TOPIC"));
		return (-1);
	}
	try {
		std::vector<Channel>::iterator channel = findChannelIt(channelName);
		if (isUserinChannel(*channel, user.getFd())) {
			if (args.size() == 2) {
				if (channel->getTopic().empty())
					user.reply(RPL_NOTOPIC(user, channelName));
				else
					user.reply(RPL_TOPIC(user, channelName, channel->getTopic()));
				return (0);
			}
			else if (isOperInChannel(user, *channel)) {
				channel->setTopic(topicStr(args));
				channel->broadcast(RPL_TOPIC(user, channelName, channel->getTopic()));
				return (0);
			}
			else
				user.reply(ERR_CHANOPRIVSNEEDED(user, channelName));
		}
		else
			user.reply(ERR_NOTONCHANNEL(user, channelName));
	}
	catch (const std::exception& e) {
		user.reply(ERR_NOSUCHCHANNEL(user, channelName));
	}
	return (-1);
}
