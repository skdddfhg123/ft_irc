#include "../Server.hpp"

static std::vector<std::string> split(std::string str, const char *delim) {
	std::vector<std::string> list;
	char 	*ptr = strtok((char *)str.c_str(), delim);
	while (ptr) {
		list.push_back(std::string(ptr));
		ptr = strtok(NULL, delim);
	}
	return list;
}

static std::string  RPL_LIST(User user, std::string channel, std::string size, std::string topic) {
	return ("323 " + user.getNick() + " " + channel + " " + size + " " + topic);
}

static std::string  RPL_LISTEND(User user) {
	return ("323 " + user.getNick() + " :End of /LIST");
}

static std::string  RPL_TOPIC(User user, std::string channel, std::string size, std::string topic) {
	return ("332 " + user.getNick() + " " + channel + " " + size + " " + topic);
}

static std::string	ERR_NOSUCHCHANNEL(User user, std::string channelName) {
	return ("403 " + user.getNick() + " " + channelName + " :No such channel");
}

int		Server::commandList(std::vector<std::string> args, User &user) {
	if (args.size() == 1 || (args.size() == 2 && args.at(1) == "\r")) {
		if (_channels.size() == 0) {
			user.reply(":NO SUCH CHANNEL");
			return (-1);
		}
		for (size_t i = 0; i < _channels.size(); i++) {
			std::stringstream strs;
			strs << _channels.at(i).getUsers().size();
			std::string size = strs.str();
			user.reply(RPL_TOPIC(user, _channels.at(i).getName(), size, _channels.at(i).getTopic()));
		}
		user.reply(RPL_LISTEND(user));
		return (0);
	}
	std::vector<std::string> channelName = split(args.at(1), ",");
	if (args.size() > 1)
	{
		for (size_t i = 0; i < channelName.size(); i++)
		{
			int flag = 0;
			size_t j = 0;
			for (; j < _channels.size() ; j++) {
				if (deleteR(channelName.at(i)) == _channels.at(j).getName()) {
					std::stringstream strs;
					strs << _channels.at(j).getUsers().size();
					std::string size = strs.str();
					user.reply(RPL_LIST(user, _channels.at(j).getName(), size, _channels.at(j).getTopic()));
					flag = 1;
					break ;
				}
			}
			if (flag == 0) {
				user.reply(ERR_NOSUCHCHANNEL(user, deleteR(channelName.at(i))));
				return (-1);
			}
		}
	}
	return (0);
}
