#include "../Server.hpp"

static std::string	ERR_NEEDMOREPARAMS(User user, std::string cmd) {
	return ("461 " + user.getNick() + " " + cmd + " :Not enough parameters");
}

static std::string	ERR_PASSWDMISMATCH(User user) {
	return ("464 " + user.getNick() + " :Password incorrect");
}

int Server::commandPass(std::vector<std::string> args, User &user) {
	if (args.size() < 2) {
		user.reply(ERR_NEEDMOREPARAMS(user, "PASS"));
		return (-1);
	}
	std::string pw = deleteR(args[1]);
	std::cout << pw << std::endl;
	if (pw != _pw) {
		// std::cout <<"pass[1] : " << "[" << pw << "]"<< std::endl;
		// std::cout << "password : " << "[" << _pw << "]" << std::endl;
		user.reply(ERR_PASSWDMISMATCH(user));
		return (-1);
	}
	user.setState(LOGIN);
	user.welcome();
	return (0);
}
