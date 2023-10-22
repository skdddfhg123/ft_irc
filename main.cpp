#include "main.hpp"
#include "srcs/Server.hpp"

Server *server;

void	sigIntHandler(int num) {
	if (num == SIGINT)
		delete server;
	exit(1);
}

void	sigQuitHandler(int num) {
	if (num == SIGQUIT)
		delete server;
	exit(1);
}

static void	check_av(char **av) {
	std::string port = av[1];
	for (std::string::iterator i = port.begin(); i != port.end(); i++) {
		if (isdigit(*i) == 0)
			throw std::runtime_error("Error: Port must consist of a number");
	}
	if (atoi(av[1]) > 65535 || atoi(av[1]) < 0) {
		throw std::runtime_error("Error: Port range is [0 to 65535]");
	}
	std::string pw = av[2];
	for (std::size_t i = 0; i < pw.size(); ++i) {
		if (!std::isalpha(pw[i]) && !std::isdigit(pw[i]))
			throw std::runtime_error("Error: Password must consist of alphabets and numbers");
	}
}

int main(int ac, char **av) {
	try {
		if (ac != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		check_av(av);
		signal(SIGINT, sigIntHandler);
		signal(SIGQUIT, sigQuitHandler);
		server = new Server(atoi(av[1]), av[2]);
		server->pollLoop();
		delete server;
		return (0);
	}
	catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return (1);
	}
}
