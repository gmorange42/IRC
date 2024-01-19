#include "../inc/Server.hpp"

void	Server::_namesCmd(int fd, Msg* msg)
{
	if (!(this->_checkAuth(fd)))
		return ;

	Client*				client = this->_clients[fd];
	std::vector<std::string>	vecNames;
	Chan*				chan = NULL;

	this->_splitNamesParams(vecNames, msg);

	if (vecNames.empty())
	{
		for (std::map<std::string, Chan*>::iterator it = this->_chans.begin();
				it != this->_chans.end(); ++it)
			this->_sendNamesRpl(it->second, client);
		this->_sendNamesRplClientsNotInAnyChan(client);
		return ;
	}

	for (std::vector<std::string>::iterator itNames = vecNames.begin();
			itNames != vecNames.end(); ++itNames)
	{
		if (!(chan = this->_findChan(*itNames, client)))
			continue ;
		this->_sendNamesRpl(chan, client);
	}
}

void	Server::_sendNamesRplClientsNotInAnyChan(Client* client)
{
	std::string	clientsNotInAnyChan("");
	std::string	chanName("*");
	bool		clientIsInChan;

	if (this->_chans.empty())
		for (std::map<int, Client*>::iterator itClients = this->_clients.begin();
				itClients != this->_clients.end(); ++itClients)
			clientsNotInAnyChan += (itClients->second->getNickname() + ' ');
	else
	{
		for (std::map<int, Client*>::iterator itClients = this->_clients.begin();
				itClients != this->_clients.end(); ++itClients)
		{
			clientIsInChan = false;
			for (std::map<std::string, Chan*>::iterator itChans = this->_chans.begin();
					itChans != this->_chans.end(); ++itChans)
			{
				if (itChans->second->isInChan(itClients->second->getFd()))
				{
					clientIsInChan = true;
					break ;
				}
			}
			if (!(clientIsInChan))
				clientsNotInAnyChan += (itClients->second->getNickname() + ' ');
		}
	}
	if (!(clientsNotInAnyChan.empty()))
		this->_sendToClient(client->getFd(), "353", RPL_NAMREPLY_353(chanName, clientsNotInAnyChan), 1);
	this->_sendToClient(client->getFd(), "366", RPL_ENDOFNAMES_366(chanName), 1);
}

void	Server::_sendNamesRpl(Chan* chan, Client* client)
{
	std::string	clientsNamesInChan("");

	for (std::map<int, Client*>::iterator it = chan->getClients()->begin();
			it != chan->getClients()->end(); ++it)
	{
		if (it != chan->getClients()->begin())
			clientsNamesInChan += ' ';
		if (chan->isOp(it->second))
			clientsNamesInChan += "@";
		clientsNamesInChan += it->second->getNickname();
	}
	this->_sendToClient(client->getFd(), "353", RPL_NAMREPLY_353(chan->getName(), clientsNamesInChan), 1);
	this->_sendToClient(client->getFd(), "366", RPL_ENDOFNAMES_366(chan->getName()), 1);
}

void	Server::_splitNamesParams(std::vector<std::string>& vecNames, Msg* msg)
{
	vecNames = splitInVector(msg->getParams(), " ");

	if (vecNames.size() == 0)
		return ;

	vecNames = splitInVector(vecNames[0], ",");
}
