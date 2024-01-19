#ifndef CHAN_HPP
# define CHAN_HPP

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <exception>

#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "Client.hpp"
#include "rpl.hpp"

class	Client;

class	Chan
{
	public:
	Chan(const std::string name, Client* client);
	~Chan(void);

	void			setName(std::string name);
	void			setTopic(std::string);

	std::string		getName(void);
	std::map<int, Client*>*	getClients(void);
	std::string		getTopic(void);
	std::string		getModes(void);
	int			getLimite(void);

	bool			checkMode(char mode);
	bool			modifOper(Client* client, char operation);
	bool			addClient(Client* client);
	bool			rmClient(int fd);
	bool			inviteClient(Client *client);
	bool			changeMode(char operation, char mode);
	bool			changeLimite(std::string& newLimite, char operation);
	bool			isInChan(int fd) const;
	bool			isOp(Client* client) const;
	bool			isInvite(Client* client) const;
	bool			chanIsFull(void) const;
	void			sendToChan(Client* client, std::string msg, int dst);

	class   SendFailedException: public std::exception
        {
                virtual const char*     what() const throw();
        };

	private:

	std::string		_name;
	std::map<int, Client*>	_clients;
	std::map<int, Client*>	_oper;
	std::map<int, Client*>	_invite;
	std::string		_modes;
	std::string		_topic;
	int			_limite;
};

#endif
