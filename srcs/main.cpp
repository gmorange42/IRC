#include <cstdlib>
#include "../inc/Server.hpp"

bool	running;

void	intHandler(int signal)
{
	(void) signal;
#ifdef DEBUG
	std::cout << "--- SIGNAL CAUGHT ---" << std::endl;
#endif
	running = false;
}

bool	checkArgs(char* argv[])
{
	int	nb;

	if (strlen(argv[1]) > 5)
		return (false);
	nb = atoi(argv[1]);
	if (nb < 1024 || nb > 65535)
		return (false);
	if (!argv[2][0])
		return (false);
	return (true);
}

int	main(int argc, char* argv[])
{
	running = false;
	if (argc != 3)
	{
		std::cerr << "Error\nUsage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}
	if (checkArgs(argv) == false)
	{
		std::cerr << "Error\nInvalid argument(s)" << std::endl;
		return (1);
	}

	Server	srv(atoi(argv[1]), argv[2]);

	signal(SIGINT, intHandler);
	signal(SIGQUIT, intHandler);
	try
	{
		srv.init();
		srv.run();
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	return (0);
}
