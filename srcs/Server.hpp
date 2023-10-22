#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <string>
# include <vector>
# include <cerrno>
# include <cstdlib>
# include <cstring>
# include <sstream>
# include <iostream>
# include <algorithm>
# include <poll.h>
# include <netdb.h>
# include <fcntl.h>
# include <unistd.h>
# include <strings.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>

# include "User.hpp"
# include "Channel.hpp"

class User;
class Channel;

class Server {
	private:
		int									_port;
		int									_socket;

		std::string							_host;
		std::string							_pw;
		std::string							_opPw;
		std::string							_servername;

		std::vector<pollfd>					_polls;
		std::vector<User>					_users;
		std::vector<std::string>			_cmds;
		std::vector<Channel>				_channels;
	public:
		Server(int port, std::string password);
		~Server();

		void								open_listenfd();
		void								pollLoop();

		void								newUser();
		void								userPrint();

		void								messageHandler(int fd);

		std::string							readMsg(int fd);
		void								parseCmd(std::string str, User &user);


		void								disconnect(int fd);
		void								deleteUser(int fd);
		void								deleteUserChannel(User &user);


		User								&findUser(int fd);
		User								&findUser(std::string nickname);
		std::vector<User>::iterator			findUserIt(int fd);
		

		bool								isChannel(std::string name);
		Channel								&findChannel(std::string name);
		std::vector<Channel>::iterator		findChannelIt(std::string name);

		int									findNick(std::string name);

		std::string							getMessage(std::vector<std::string> args);
		std::string							deleteR(std::string str);

		int 								commandUser(std::vector<std::string> args, User &user);
		int									commandNick(std::vector<std::string> args, User &user);
		int									commandJoin(std::vector<std::string> args, User &user);
		int									commandPass(std::vector<std::string> args, User &user);
		int									commandOper(std::vector<std::string> args, User &user);
		int									commandPrimsg(std::vector<std::string> args, User &user);
		int									commandNotice(std::vector<std::string> args, User &user);
		int									commandKill(std::vector<std::string> args, User &user);
		int									commandPing(std::vector<std::string> args, User &user);
		int									commandPart(std::vector<std::string> args, User &user);
		int									commandKick(std::vector<std::string> args, User &user);
		int									commandTopic(std::vector<std::string> args, User &user);
		int									commandList(std::vector<std::string> args, User &user);
		int									commandMode(std::vector<std::string> args, User &user);
};

#endif
