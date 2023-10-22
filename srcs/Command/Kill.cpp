#include "../Server.hpp"

static std::string	ERR_NOSUCHNICK(User user, std::string nick) {
	return ("401 " + user.getNick() + " " + nick + " :No such nick");
}

static std::string	ERR_NEEDMOREPARAMS(User user, std::string cmd) {
	return ("461 " + user.getNick() + " " + cmd + " :Not enough parameters");
}

static std::string	ERR_CHANNELISFULL(User user, std::string cmd) {
	return ("481 " + user.getNick() + " " +  cmd + " :Permission Denied- You're not an IRC operator");
}

int	Server::commandKill(std::vector<std::string> args, User &user) {
	std::string	cmd = args.at(0);
	size_t		i = 0;
	int			check = 0;

	if (user.getIsAuth() == false) {
		user.reply(ERR_CHANNELISFULL(user, cmd));
		return (-1);
	}
	if (args.size() < 2) {
		user.reply(ERR_NEEDMOREPARAMS(user, cmd));
		return (-1);
	}
	for (i = 0; i < _users.size(); i++) {
		if (args.at(1) == _users.at(i).getNick()) {
			check = 1;
			break ;
		}
	}
	if (check != 1) {
		user.reply(ERR_NOSUCHNICK(user, args.at(1)));
		return (-1);
	}
	disconnect(_users.at(i).getFd());
	return (0);
}
