#include "Channel.hpp"
#include "Server.hpp"

Channel::Channel(std::string name) 
: _fdAuth(0), _maxUser(0), _pw(""), _name(name), _topic() { }

Channel::~Channel() { }

int						Channel::getFdAuth() const { return _fdAuth; }
size_t 					Channel::getMaxUser() const { return _maxUser; }
std::string				Channel::getName() const { return _name; }
std::string				Channel::getTopic() const { return _topic; }
std::string				Channel::getPassword() const { return _pw; }
std::vector<User>		&Channel::getUsers() { return _users; }

void					Channel::setFdAuth(int fd) { _fdAuth = fd; }
void					Channel::setMaxUser(int maxUser) { _maxUser = maxUser; }
void					Channel::setTopic(std::string newTopic) { _topic = newTopic; }
void					Channel::setPassword(std::string pass) { _pw = pass; }

void					Channel::addUser(User &user) { _users.push_back(user); }

void	Channel::deleteUser(User &user) {
	std::vector<User>::iterator	it;
	for (it = _users.begin(); it != _users.end(); it++) {
		std::cout << it->getNick() << "==" << user.getNick() << std::endl;
		if (it->getFd() == user.getFd()) {
			std::cout << "delete user" << std:: endl;
			std::string ret = user.getPrefix() + " PART :" + _name;
			broadcast(ret);
			_users.erase(it);
			return ;
		}
	}
}

void	Channel::broadcast(std::string msg) {
	msg += "\r\n";
	std::cout << "== " << msg << std::endl;
	for (unsigned int i = 0; i < _users.size(); i++) {
		if (send(_users[i].getFd(), msg.c_str(), msg.length(), 0) < 0)
			throw std::runtime_error("Error: broadcasting");
	}
}

void	Channel::broadcast(std::string msg, User &user) {
	msg += "\r\n";
	std::cout << "== " << msg << std::endl;
	for (unsigned int i = 0; i < _users.size(); i++) {
		if (user.getFd() != _users[i].getFd()) {
			if (send(_users[i].getFd(), msg.c_str(), msg.length(), 0) < 0)
				throw std::runtime_error("Error: broadcasting");
		}
	}
}
