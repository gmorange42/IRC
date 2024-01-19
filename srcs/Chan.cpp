#include "../inc/Chan.hpp"

Chan::Chan(const std::string name, Client* client) : _name(name), _modes("nt"), _topic(""), _limite(-1)
{
#ifdef DEBUG
	std::cout << "Chan constructor called" << std::endl;
#endif
	this->modifOper(client, '+');
	this->addClient(client);
}

Chan::~Chan(void)
{
#ifdef DEBUG
	std::cout << "Chan destructor called" << std::endl;
#endif
}

void	Chan::setName(std::string name)
{
	this->_name = name;
}

void	Chan::setTopic(std::string topic)
{
	this->_topic = topic;
}

std::string	Chan::getName(void)
{
	return (this->_name);
}

std::map<int, Client*>*	Chan::getClients(void)
{
	return (&this->_clients);
}

std::string	Chan::getTopic(void)
{
	return (this->_topic);
}

std::string	Chan::getModes(void)
{
	return (this->_modes);
}

int	Chan::getLimite(void)
{
	return (this->_limite);
}

bool	Chan::checkMode(char mode)
{
	if ((this->_modes.find(mode)) == std::string::npos)
		return (false);
	return (true);
}

bool	Chan::modifOper(Client* client, char operation)
{
	if (operation == '+')
	{
		if ((this->_oper.count(client->getFd()) != 0))
			return (false);
		this->_oper.insert(std::make_pair(client->getFd(), client));
	}
	else
	{
		if ((this->_oper.count(client->getFd()) == 0))
			return (false);
		this->_oper.erase(client->getFd());
	}
	return (true);
}

bool	Chan::addClient(Client* client)
{	
	if (!this->isInChan(client->getFd()))
	{
		this->_clients.insert(std::make_pair(client->getFd(), client));
		this->_invite.erase(client->getFd());
		return (true);
	}
	return (false);
}

bool	Chan::rmClient(int fd)
{
	this->_clients.erase(fd);
	this->_oper.erase(fd);
	this->_invite.erase(fd);
	if (this->_clients.size() > 0)
		return (true);
	return (false);
}

bool	Chan::inviteClient(Client *client)
{
	if (!client)
		return (false);
	if (!this->isInvite(client))
	{
		this->_invite.insert(std::make_pair(client->getFd(), client));
		return (true);
	}
	return (false);
}

bool	Chan::changeMode(char operation, char mode)
{
	if (operation == '+' && (!this->checkMode(mode)))
	{
		this->_modes.push_back(mode);
		return (true);
	}
	else if (operation == '-' && (this->checkMode(mode)))
	{
		this->_modes.erase(this->_modes.begin() + this->_modes.find(mode));
		return (true);
	}
	return (false);
}

bool	Chan::changeLimite(std::string& newLimite, char operation)
{
	if (operation == '-')
	{
		if (this->_limite == -1)
			return (false);
		this->_limite = -1;
	}
	else
	{
		if (newLimite.empty())
			return (false);
		if (this->_limite == atoi(newLimite.c_str()))
			return (false);
		this->_limite = atoi(newLimite.c_str());
		if (this->_limite == 0)
			newLimite = "0";
	}
	this->changeMode(operation, 'l');
	return (true);
}

bool	Chan::isInChan(int fd) const
{
	if ((this->_clients.count(fd)) == 0)
		return (false);
	return (true);
}

bool	Chan::isOp(Client* client) const
{
	if (this->_oper.count(client->getFd()) == 0)
		return (false);
	return (true);
}

bool	Chan::isInvite(Client* client) const
{
	if (!client)
		return (true);
	if (this->_invite.count(client->getFd()) == 0)
		return (false);
	return (true);
}

bool	Chan::chanIsFull(void) const
{
	if (this->_limite > -1 && (static_cast<int>(this->_clients.size()) >= this->_limite))
		return (true);
	return (false);
}

void	Chan::sendToChan(Client* client, std::string msg, int dst)
{
	std::map<int, Client*>::iterator	it;

#ifdef DEBUG
	std::cout << "--- in chan sendToChan ---" << std::endl;
	std::cout << "msg: " << msg << std::endl;
#endif

	for (it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (dst == 3 || (dst == 2 && *(it->second) != *client) || !client)
		{
			if (send(it->second->getFd(), msg.c_str(), msg.size(), MSG_NOSIGNAL) < 0)
				throw Chan::SendFailedException();
		}
		else if (dst == 1)
		{
			if (this->isOp(it->second) && *(it->second) != *client)
			{
				if (send(it->second->getFd(), msg.c_str(), msg.size(), MSG_NOSIGNAL) < 0)
					throw Chan::SendFailedException();
			}
		}
	}
}

const char*	Chan::SendFailedException::what() const throw()
{
	return ("Call to send() failed");
}
