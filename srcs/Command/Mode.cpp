#include "../Server.hpp"
#include "../User.hpp"
#include "../Channel.hpp"

static std::string	deleteR(std::string str) {
	if (str.empty())
		return "";
	if (str[str.size() - 1] == '\r')
		return str.substr(0, (str.size() - 1));
	return str;
}

static std::string	ERR_NOSUCHNICK(User user, std::string nick) {
	return ("401 " + user.getNick() + " " + nick + " :No such nick");
}

static std::string	ERR_NOSUCHCHANNEL(User user, std::string channelName) {
	return ("403 " + user.getNick() + " " + channelName + " :No such channel");
}

static std::string	ERR_NEEDMOREPARAMS(User user, std::string cmd) {
	return ("461 " + user.getNick() + " " + cmd + " :Not enough parameters");
}

static std::string	ERR_CHANOPRIVSNEEDED(User user, std::string channel) {
	return ("482 " + user.getNick() + " " + channel + " :You're not channel operator");
}

static std::string	ERR_UMODEUNKNOWNFLAG(User user) {
	return ("501 " + user.getNick() + " :Unknown MODE flag");
}

static int	oFlag(User &user, std::vector<std::string> args, Channel &channel) {
	int		check = 0;
	size_t	i = 0;
	for (; i < channel.getUsers().size(); i++) {
		if (channel.getUsers().at(i).getNick() == deleteR(args.at(3))) {
			check = 1;
			break ;
		}
	}
	if (check == 1)
		channel.setFdAuth(channel.getUsers().at(i).getFd());
	else
		user.reply(ERR_NOSUCHNICK(user, deleteR(args.at(3))));
	return (0);
}

int	Server::commandMode(std::vector<std::string> args, User &user) {
	std::cout << "in MODE->args = " << std::endl;
	for (unsigned int i = 0; i < args.size(); i++)
		std::cout << args[i] << std::endl ;
	std::cout << std::endl;

	if (args.size() < 4) {
		user.reply(ERR_NEEDMOREPARAMS(user, "MODE"));
		return (-1);
	}
	if (isChannel(args.at(1)) == false) {
		if (deleteR(args[1]).at(0) != '#')
			return (-1);
		user.reply(ERR_NOSUCHCHANNEL(user, deleteR(args[1])));
		return (-1);
	}
	if (user.getFd() != findChannel(args.at(1)).getFdAuth()) {
		user.reply(ERR_CHANOPRIVSNEEDED(user, args.at(1)));
		return (-1);
	}
	Channel	channel = findChannel(args.at(1));
	int		i = 0;
	std::string	flags[3] = {"+O","+o","-o"};

	std::string	flag = deleteR(args[2]);
	while (i < 3 && flag != flags[i])
		i++;
	std::cout << "CHECK_FLAG" << std::endl;
	switch (i)
	{
		case 0:
			return(oFlag(user, args, channel));
		case 1:
			return(oFlag(user, args, channel));
		case 2:
			user.reply(ERR_CHANOPRIVSNEEDED(user, channel.getName()));
			return (-1);
		default:
			user.reply(ERR_UMODEUNKNOWNFLAG(user));
			return (-1);
	}
	return (1);
}
