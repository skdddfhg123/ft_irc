#ifndef USER_HPP
# define USER_HPP

# include <vector>
# include <string>
# include <fstream>
# include <sstream>
# include <iostream>
# include <sys/socket.h>

# include "Channel.hpp"
# include "Server.hpp"

enum	State {
	HANDSHAKE,
	LOGIN,
	REGISTERED
};

class User {
	private:
		int						_fd;
		bool					_isAuth;
		std::string				_host;
		std::string				_nick;
		std::string				_real;	
		std::string				_user;
		std::string				_msg;
		State					_state;		
	public:
		User(int fd, std::string host);
		~User();

		std::string  	getPrefix(); 
		void			reply(std::string msg);
		void			welcome();
	// Getter
		int				getFd(void) const;
		bool			getIsAuth(void) const;
		State			getState(void) const;
		std::string		getHost(void) const;
		std::string		getNick(void) const;
		std::string		getReal(void) const;
		std::string		getUser(void) const;
		std::string		getMsg(void) const;
	// Setter
		void			setAuth(bool isAuth);
		void			setState(State state);
		void			setHost(std::string host);
		void			setNick(std::string nick);
		void			setReal(std::string real);
		void			setUser(std::string user);
		void			setMsg(std::string msg);
		void			addMsg(std::string buff);
};

#endif
