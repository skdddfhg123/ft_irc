#include "User.hpp"

User::User(int fd, std::string host) : _fd(fd), _isAuth(false), _host(host) {
	_state = HANDSHAKE;
	_nick = "";
	_user = "";
	_real = "";
	_msg = "";
}

User::~User() { }

void	User::reply(std::string msg) {
	std::string	userF = "";
	std::string	hostF = "";
	if (!(_user.empty()))
		userF = "!";
	if (!(_host.empty()))
		hostF = "@";
	std::string prefix = _nick + userF + hostF;
	std::string packet = ":" + prefix + " " + msg + "\r\n";
	std::cout << ">>>>> " << packet << std::endl;
	if (send(_fd, packet.c_str(), packet.length(), 0) < 0)
		throw(std::out_of_range("Error while sending"));
}

void	User::welcome() {
	if (_state != LOGIN || _nick.empty() || _user.empty()) {
		std::cout << "Wait Register" << _nick << std::endl;
		return ;
	}
	_state = REGISTERED;
	reply("001 " + _nick + " :Welcome " +_nick +  " into our irc network");
	std::cout << _nick << " is registered" << std::endl;
}

// Getter
int			User::getFd(void) const { return _fd; }
bool		User::getIsAuth() const { return _isAuth; }
State		User::getState(void) const { return _state; }
std::string	User::getNick(void) const { return _nick; }
std::string	User::getUser(void) const { return _user; }
std::string	User::getReal(void) const { return _real; }
std::string	User::getHost(void) const { return _host; }
std::string	User::getMsg(void) const { return _msg; }

// Setter
void		User::setAuth(bool isAuth) { _isAuth = isAuth; }
void		User::setState(State state) { _state = state; }
void		User::setHost(std::string host) { _host = host; }
void		User::setNick(std::string nick) { _nick = nick; }
void		User::setUser(std::string user) { _user = user; }
void		User::setReal(std::string real) { _real = real; }
void		User::setMsg(std::string msg) { _msg = msg; }
void		User::addMsg(std::string buff) { _msg += buff; }

std::string  User::getPrefix() {
	std::string	userF = "";
	std::string	hostF = "";
	if (!(_user.empty()))
		userF = "!";
	if (!(_host.empty()))
		hostF = "@";
	std::string prefix = ":" + _nick + userF + hostF;;
	return (prefix);
}
