#include "../inc/Server.hpp"

Chan*	Server::_findChan(std::string chanName, Client* client)
{
	Chan*	chan = NULL;
	std::map<std::string, Chan*>::iterator	itChan;

	if (chanName[0] == ':')
		chanName.erase(chanName.begin());

	if ((itChan = this->_chans.find(lower(chanName))) == this->_chans.end())
		this->_sendToClient(client->getFd(), "403", ERR_NOSUCHCHANNEL_403(chanName), 1);
	else
		chan = itChan->second;
	return (chan);
}

bool	Server::_checkClientInChan(Chan* chan, Client* receiver, Client* clientToCheck)
{
	if (receiver == clientToCheck)
	{
		if (chan->isInChan(receiver->getFd()))
			return (true);
		this->_sendToClient(receiver->getFd(), "442", ERR_NOTONCHANNEL_442(chan->getName()), 1);
	}
	else
	{
		if (chan->isInChan(clientToCheck->getFd()))
			return (true);
		this->_sendToClient(receiver->getFd(), "441", ERR_USERNOTINCHANNEL_441(clientToCheck->getNickname(), chan->getName()), 1);
	}
	return (false);
}

bool	Server::_checkClientNotInChan(Chan* chan, Client* receiver, Client* clientToCheck)
{
	if (!(chan->isInChan(clientToCheck->getFd())))
		return (true);
	this->_sendToClient(receiver->getFd(), "443", ERR_USERONCHANNEL_443(clientToCheck->getNickname(), chan->getName()), 1);
	return (false);
}

Client*	Server::_findClientInServer(std::string clientName, Client* client)
{
	Client* clientToCheck = NULL;

	for (std::map<int, Client*>::iterator it = this->_clients.begin();
			it != this->_clients.end(); ++it)
		if ((it->second->getNickname().compare(clientName)) == 0)
			clientToCheck = it->second;
	if (!clientToCheck)
		this->_sendToClient(client->getFd(), "401", ERR_NOSUCHNICK_401(clientName), 1);
	return  (clientToCheck);
}

bool	Server::_checkClientIsOp(Chan *chan, Client* client)
{
	if (chan->isOp(client))
		return (true);
	this->_sendToClient(client->getFd(), "482", ERR_CHANOPRIVSNEEDED_482(chan->getName()), 1);
	return (false);
}

bool	Server::_checkNbParams(std::vector<std::string>& vec, size_t size, Client* client, Msg* msg)
{
	if (vec.size() >= size)
		return (true);
	this->_sendToClient(client->getFd(), "461", ERR_NEEDMOREPARAMS_461(msg->getCmd()), 1);
	return (false);
}
