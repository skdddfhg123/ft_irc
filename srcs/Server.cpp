#include "Server.hpp"

Server::Server(int port, std::string pw) 
: _port(port), _host("127.0.0.1"), _pw(pw), _opPw("admin") {
	open_listenfd();
}

Server::~Server(void) { }

/*
// IP socket address structure
struct sockaddr_in {                // IPv4 주소를 저장하는 구조체
	uint16_t        sin_family;     // 주소 체계를 저장하는 필드(AF_INET)
	uint16_t        sin_port;       // 포트 정보 저장
	struct in_addr  sin_addr;       // IPv4 주소 저장
	unsigned char   sin_zero[8];    // 사용하지 않는 필드. 0으로 채움
};
// Generic socket address structure (for connect, bind, and accept)
struct sockaddr {
	uint16_t       sa_family;       // 주소 체계
	char           sa_data[4];      // 해당 주소 체계에서 사용하는 주소 정보(IP정보 + 포트 정보)
};
*/
void Server::open_listenfd() {
	// socket 생성 함수 (int domain, int type, int protocol) return 성공 0이상 fd값, 실패 -1
	// domain=AF_INET(inetnetwork), type=SOCK_STREAM(TCP stream 소켓), IPPROTO_TCP(TCP 사용))
	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error("Error: socket failed");
	// 소켓 옵션에서 bind시에 local 주소를 재사용할 것인지 여부 true로 setting
	// setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen); return 성공0 실패 -1
	int optval = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		throw std::runtime_error("Error: setsockopt failed");
	// socket fd를 비동기로 설정 (Mac only)
	if (fcntl(this->_socket, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Error: fcntl failed");

	sockaddr_in	serv_addr = {};
	std::memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(this->_port);
	
	// int bind(int sockfd, struct sockaddr *addr, socklen_t addrlen); return 성공 0, 실패 -1
	// 소켓이랑 서버 정보를 묶어 주는 함수이다. 아래는 서버 정보를 servinfo 구조체로 끌고 왔기에 그냥 맞춰 넣음
	if (bind(_socket, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr)) == -1)
		throw std::runtime_error("Error: binding failed");
	if (listen(_socket, SOMAXCONN) == -1)
		throw std::runtime_error("Error: listening failed");
	// 벡터 polls의 0번 인덱스 위치는 연결형 소켓을 위함 -> accept 여부 체크
	pollfd pfd = {_socket, POLLIN, 0};
	_polls.push_back(pfd);
	std::cout << "Server is listening on port " << _port << std::endl;
}

void	Server::pollLoop()
{
	while(1) {
		// int poll(struct pollfd * fds, nfds_t nfds, int timeout); return event발생한 fd갯수, 0: timeout, -1: error
		// nfds: fd개수, timeout-> -1: 무한대기, 0: 대기없이 종료
		if (poll(_polls.begin().base(), _polls.size(), -1) == -1)
			break ;
		for (unsigned int i = 0; i < _polls.size(); i++) {
			if (_polls[i].revents == 0)
				continue ;
			// event 발생
			if ((_polls[i].revents  & POLLIN ) == POLLIN) {
				// 유저 등록
				if (_polls[i].fd == _socket) {
					sockaddr_in			client_addr = {};
					socklen_t			client_addr_size = sizeof(client_addr);
					int					user_fd;
					char				hostname[4096];

					user_fd = accept(_socket, (sockaddr *)&client_addr , &client_addr_size);
					if (user_fd == -1)
						return ;
					if (getnameinfo((struct sockaddr *) &client_addr, sizeof(client_addr), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0)
						throw std::runtime_error("Error: while getting hostname on new client.");
					_users.push_back(User(user_fd, hostname));
					pollfd	polls = {user_fd, POLLIN, 0};
					_polls.push_back(polls);
					userPrint();
					break;
				}
			}
		messageHandler(_polls[i].fd);
		}
	}
	for (size_t i = 0; i < _polls.size(); i++)
		close(_polls[i].fd);
}

static std::vector<std::string>	splitCmd(std::string msg) {
	std::vector<std::string>	cmd;
	std::stringstream			str(msg);
	std::string					tmp;

	int i = 0;
	if (msg == "\n")
		return cmd;
	while (std::getline(str, tmp, '\n')) {
		cmd.push_back(tmp);
		std::cout << cmd.at(i++) << std::endl;
	}
	return (cmd);
}

std::string		Server::readMsg(int fd) {
	std::string					msg;
	std::vector<User>::iterator user = findUserIt(fd);
	char						buf[4096];

	std::memset(buf, 0, 4096);
	msg = user->getMsg();
	while (!std::strstr(buf, "\n")) {
		int i = 0;
		std::memset(buf, 0, 4096);
		// ssize_t recv(int sockfd, void *buf, size_t len, int flags);
		if ((i = recv(fd, buf, 4096, MSG_DONTWAIT)) < 0) {
			if (errno == EAGAIN)
				return ("");
			throw std::runtime_error("Error: receive");
		}
		// else if (i == 0) {
		// 	throw(std::out_of_range("disconnect client(test)"));
		// }
		user->addMsg(buf);
		msg += buf;
	}
	user->setMsg("");
	return (msg);
}

void	Server::messageHandler(int fd) {
	try {
		_cmds = splitCmd(readMsg(fd));
	}
	catch(const std::exception& e) {
		disconnect(fd);
		std::cerr << e.what() << '\n';
		return ;
	}
	for (std::vector<std::string>::iterator it = this->_cmds.begin(); it != this->_cmds.end(); it++)
		parseCmd(*it, findUser(fd));
	userPrint();
}

void	Server::parseCmd(std::string str, User &user) {
	std::string					cmd;
	std::vector<std::string>	args;
	std::stringstream 			ss(str);
	std::getline(ss, cmd, ' ');

	args.push_back(cmd);
	std::cout << "Parssing command == [" << cmd << "]" << std::endl;

	std::string cmds[14] = {"PASS", "NICK", "OPER", "USER", "PRIVMSG", "JOIN", "kill", "PING", "PART", "LIST", "TOPIC", "KICK", "MODE", "NOTICE"};

	int		(Server::*ptr[14])(std::vector<std::string> args, User &user) = {
			&Server::commandPass,
			&Server::commandNick,
			&Server::commandOper,
			&Server::commandUser,
			&Server::commandPrimsg,
			&Server::commandJoin,
			&Server::commandKill,
			&Server::commandPing,
			&Server::commandPart,
			&Server::commandList,
			&Server::commandTopic,
			&Server::commandKick,
			&Server::commandMode,
			&Server::commandNotice,
	};

	for (int i = 0; i < 14; ++i) {
		if (cmd == cmds[i]) {
			while (std::getline(ss, cmd, ' '))
				args.push_back(cmd);
			(this->*ptr[i])(args, user);
			return ;
		}
	}
}

void	Server::userPrint() {
	std::string state;
	unsigned int	i = _users.size();
	std::cout << "User connected : " << i << std::endl;
	for (unsigned int j = 0; j < i; j++) {
		if (_users.at(j).getState() == REGISTERED) {
			state =  "yes";
		}
		else {
			state =  "no";
		}
		std::cout << "user[" << j << "]		registered:" << state
		<< "	nick:" << _users.at(j).getNick() 
		<< "	user:" << _users.at(j).getUser()
		<< "	realname:" << _users.at(j).getReal() << std::endl;
	}
	std::cout << std::endl;
}

void	Server::deleteUser(int fd) {
	std::vector<User>::iterator it = _users.begin();
	while (it != _users.end()) {
		if (it->getFd() == fd) {
			_users.erase(it);
			return ;
		}
		it++;
	}
}

void	Server::disconnect(int fd)
{
	std::vector<pollfd>::iterator pollIt = _polls.begin();
	deleteUserChannel(findUser(fd));
	deleteUser(fd);

	while (pollIt != _polls.end()) {
		if (pollIt->fd == fd) {
			_polls.erase(pollIt);
			break;
		}
		pollIt++;
	}
	close(fd);
	std::cout << "Disconnect complete" << std::endl;
}

void	Server::deleteUserChannel(User &user)
{
	for (unsigned int i = 0; i < _channels.size(); i++) {
		_channels[i].deleteUser(user);
	}
	std::vector<Channel>::iterator   it = _channels.begin();
	while (it != _channels.end())
	{
		if (it->getUsers().empty())
			it = _channels.erase(it);
		else
			it++;
	}
}

User		&Server::findUser(int fd) {
	for (unsigned int i = 0; i < _users.size(); i++) {
		if (_users[i].getFd() == fd)
			return (_users[i]);
	}
	throw(std::out_of_range("Error: search user"));
}

User		&Server::findUser(std::string nick) {
	for (unsigned int i = 0; i < _users.size(); i++) {
		if (_users[i].getNick() == nick)
			return (_users[i]);
	}
	throw(std::out_of_range("Error: search user"));
}

std::vector<User>::iterator	Server::findUserIt(int fd) {
	std::vector<User>::iterator ret = _users.begin();
	std::vector<User>::iterator end = _users.end();
	while (ret != end) {
		if (ret->getFd() == fd)
			return (ret);
		ret++;
	}
	throw(std::out_of_range("Error: search user"));
}

bool		Server::isChannel(std::string name) {
	for (unsigned int i = 0; i < _channels.size(); i++) {
		if (_channels[i].getName() == name)
			return true;
	}
	return false;
}

Channel		&Server::findChannel(std::string name) {
	for (unsigned int i = 0; i < _channels.size(); i++) {
		if (_channels[i].getName() == name)
			return (_channels[i]);
	}
	throw (std::out_of_range("Error: search channel"));
}

std::vector<Channel>::iterator Server::findChannelIt(std::string name) {
	std::vector<Channel>::iterator  ret = _channels.begin();
	std::vector<Channel>::iterator  end = _channels.end();
	while (ret != end) {
		if (ret->getName() == name)
			return (ret);
		ret++;
	}
	throw (std::out_of_range("Error: search channel"));
}

int	Server::findNick(std::string name)
{
	for (std::vector<User>::iterator i = _users.begin(); i != _users.end(); i++)
	{
		if ((i)->getNick() == name)
			return ((i)->getFd());
	}
	return 0;
}

std::string	Server::getMessage(std::vector<std::string> args) {
	std::string msg;

	unsigned int i = 3;
	msg = args[2];
	if (msg[0] == ':')
		msg = msg.substr(1);
	while (i < args.size()) {
		msg += " ";
		msg += args[i];
		i++;
	}
	msg = deleteR(msg);
	return (msg);
}

std::string	Server::deleteR(std::string str) {
	if (str.empty())
		return "";
	if (str[str.size() - 1] == '\r')
		return str.substr(0, (str.size() - 1));
	return str;
}
