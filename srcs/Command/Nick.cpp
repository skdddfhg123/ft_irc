#include "../Server.hpp"

static bool	isSpecial(char c) {
	if (c >= 0x5B && c <= 0x60)
		return (true);
	else if (c >= 0x7B && c <= 0x7D)
		return (true);
	else
		return (false);
}

static bool	valid_nickname(std::string name) {
	if (!(isSpecial(name[0]) || isalpha(name[0])))
		return (false);
	for (size_t i = 1; i < name.size(); ++i)
	{
		if (!(isalnum(name[i]) || isSpecial(name[i]) || name[i] == '-'))
			return (false);
	}
	return (true);
}

static std::string	ERR_NONICKNAMEGIVEN(User user) {
	return ("431 " + user.getNick() + " :No nickname given");
}

static std::string	ERR_ERRONEUSNICKNAME(User user, std::string newNick) {
	return ("432 " + user.getNick() + " " + newNick + " :Erroneus nickname");
}

static std::string	ERR_NICKNAMEINUSE(User user, std::string newNick) {
	return ("433 " + user.getNick() + " " + newNick + " " + newNick + " :Nickname is already in use");
}

int	Server::commandNick(std::vector<std::string> args, User &user)
{
	if (args.size() < 2) {
		user.reply(ERR_NONICKNAMEGIVEN(user));
		return (-1);
	}
	std::string	newNick = deleteR(args[1]); // pass 끝에 있는 \r 제거
	if (valid_nickname(newNick) == false) {
		user.reply(ERR_ERRONEUSNICKNAME(user, newNick));
		return (-1);
	}
	if (findNick(newNick) > 0) {
		user.reply(ERR_NICKNAMEINUSE(user, newNick));
		return (-1);
	}
	user.reply(" NICK " + newNick);
	user.setNick(newNick);
	user.welcome();
	return (0);
}
