#include "../Server.hpp"

static std::string	ERR_NEEDMOREPARAMS(User &user, std::string cmd) {
	return ("461 " + user.getNick() + " " + cmd + " :Not enough parameters");
}

static std::string	ERR_ALREADYREGISTRED(User &user, std::string cmd) {
	return ("462 " + user.getNick() + " " + cmd + " :Unauthorized command (already registered)");
}

int Server::commandUser(std::vector<std::string> args, User &user) {
	std::string tmp;
	std::string cmd = args.at(0);
	if (args.size() < 3) {
		user.reply(ERR_NEEDMOREPARAMS(user, "USER"));
		return (-1);
	}
	else if (user.getUser() == args.at(1)) {
		user.reply(ERR_ALREADYREGISTRED(user, "USER"));
		return (-1);
	}
	else if (args.size() >= 3) {
		user.setUser(args.at(1));
		size_t i;
		for (i = 1; i < args.size() - 1; i++) {
			if (args.at(i)[0] == ':')
				break ;
		}
		if (args.at(1)[0] == ':')
			tmp = args.at(i).substr(1);
		else
			tmp = args.at(i);
		if (i + 1 == args.size()) {
			user.setReal(tmp);
			user.welcome();
			return (0);
		}
		i++;
		tmp += " ";
		for (; i < args.size() - 1; i++)
			tmp += args.at(i) + " ";
		tmp += args.at(i);
		user.setReal(tmp);
	}
	user.welcome();
	return 0;
}
