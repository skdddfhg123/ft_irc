#include "../Server.hpp"

static std::string	ERR_NEEDMOREPARAMS(User user, std::string cmd) {
	return ("461 " + user.getNick() + " " + cmd + " :Not enough parameters");
}

static std::string	ERR_PASSWDMISMATCH(User user) {
	return ("464 " + user.getNick() + " :Password incorrect");
}

static std::string	RPL_YOUREOPER(User user, std::string cmd) {
	return ("381 " + user.getNick() + " " + cmd + " :you are now an IRC operator");
}

int Server::commandOper(std::vector<std::string> args, User &user) {
	for (size_t i = 0; i < args.size(); i++)
		std::cout << args.at(i) << std::endl;
	std::string cmd = args.at(0);
	if (args.size() < 3) {
		user.reply(ERR_NEEDMOREPARAMS(user, "OPER"));
		return (-1);
	}
	std::string opPw = deleteR(args.at(2));
	if (_opPw != opPw) {
		user.reply(ERR_PASSWDMISMATCH(user));
		return (-1);
	}
	else {
		user.setAuth(true);
		user.reply(RPL_YOUREOPER(user, "OPER"));
	}
	return (0);
}
