#include "../inc/Server.hpp"

void	Server::_nickCmd(int fd, Msg* msg)
{
	if (!this->_checkNick(fd, msg))
		return ;
	if (msg->getParams().size() > static_cast<long unsigned>(atoi(NICKLEN)))
		msg->setParams(msg->getParams().substr(0, atoi(NICKLEN)));

	std::map<int, Client*>::iterator	it;
	std::string				nickname = lower(msg->getParams());

	for (it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (nickname.compare(lower(it->second->getNickname())) == 0 && it->second->getFd() != fd)
		{
			this->_sendToClient(fd, "433", ERR_NICKNAMEINUSE_433(msg->getParams()), 1);
			return ;
		}
	}

	this->_clients[fd]->setNickname(msg->getParams());

	if (this->_clients[fd]->getAuth() == true)
	{
		this->_sendToClient(fd, "NICK", msg->getParams(), 2);
		this->_sendToChan(fd, "NICK", msg->getParams());
	}

	this->_clients[fd]->setFullId();
	if (this->_clients[fd]->getAuth() == false && !this->_clients[fd]->getNickname().empty() && !this->_clients[fd]->getUsername().empty())
		this->_checkPass(fd, msg);
}

bool	Server::_checkNick(int fd, Msg* msg)
{
	if (msg->getParams().empty())
	{
		this->_sendToClient(fd, "431", ERR_NONICKNAMEGIVEN_431, 1);
		return (false);
	}
	if (msg->getParams()[0] == ':')
		msg->getParams() = msg->getParams().substr(1);

	std::string		nickname;

	split(nickname, msg->getParams(), " ");

	if (!nickname.empty())
		msg->getParams() = nickname;

	if (checkChars(msg->getParams(), true) == false)
	{
		this->_sendToClient(fd, "432", ERR_ERRONEUSNICKNAME_432(msg->getParams()), 1);
		return (false);
	}
	return (true);
}
