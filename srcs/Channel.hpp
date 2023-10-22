#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "User.hpp"
# include "Server.hpp"

class User;

class Channel {
	private:
		int							_fdAuth;
		int							_maxUser;
		std::string					_pw;
		std::string					_name;
		std::string					_topic;
		std::vector<User>			_users;
	public:
		Channel(std::string name);
		~Channel();

	// Getter
		int						getFdAuth() const; 
		size_t					getMaxUser() const;
		std::string				getPassword() const;
		std::string				getName() const;
		std::string				getTopic() const;
		std::vector<User>		&getUsers();
	// Setter
		void					setFdAuth(int fd);
		void					setMaxUser(int maxUser);
		void					setTopic(std::string topic);
		void					setPassword(std::string pw);
	// Channel functions
		void					addUser(User &user);
		void					deleteUser(User &user);

		void					broadcast(std::string message);
		void					broadcast(std::string message, User &cl);
};

#endif
