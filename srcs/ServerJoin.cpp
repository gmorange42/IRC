#include "../inc/Server.hpp"

void	Server::_joinCmd(int fd, Msg* msg)
{
	if (!(this->_checkAuth(fd)))
		return ;

	Client*					client = this->_clients[fd];
	std::vector<std::string>		vecNames;
	std::map<std::string, Chan*>::iterator	itChan;

	if (!(this->_splitJoinParams(vecNames, msg, client)))
		return ;

	for (size_t i = 0; i < (vecNames.size()); ++i)
	{
		if (vecNames[i].size() == 0 || vecNames[i].size() > 50 || vecNames[i].at(0) != '#')
		{
			this->_sendToClient(client->getFd(), "403", ERR_NOSUCHCHANNEL_403(vecNames[i]), 1);
			continue ;
		}

		itChan = this->_chans.find(lower(vecNames[i]));

		if (itChan == this->_chans.end())
		{
			if (this->_chans.size() >= static_cast<unsigned long>(atoi(MAXCHAN)))
			{
				this->_sendToClient(client->getFd(), "403", ERR_NOSUCHCHANNEL_403(vecNames[i]), 1);
				continue ;
			}
			Chan*	chan = new Chan(vecNames[i], client);
			this->_chans.insert(std::pair<std::string, Chan*>(lower(vecNames[i]), chan));
			this->_sendJoinRpls(client, this->_chans[lower(vecNames[i])]);
		}
		else
		{
			if (this->_checkJoinErrors(itChan->second, client))
				if (itChan->second->addClient(client))
					this->_sendJoinRpls(client, itChan->second);
		}
	}
}

bool	Server::_splitJoinParams(std::vector<std::string>& vecNames, Msg* msg, Client* client)
{
	vecNames = splitInVector(msg->getParams(), " ");

	if (!(this->_checkNbParams(vecNames, 1, client, msg)))
		return (false);

	vecNames = splitInVector(vecNames[0], ",");
	return (true);
}

bool	Server::_checkJoinErrors(Chan *chan, Client* client)
{	
	if (chan->checkMode('i') && (chan->isInvite(client) == false))
	{
		this->_sendToClient(client->getFd(), "473", ERR_INVITEONLYCHAN_473(chan->getName()), 1);
		return (false);
	}
	else if (chan->chanIsFull() && (chan->isInvite(client) == false))
	{
		this->_sendToClient(client->getFd(), "471", ERR_CHANNELISFULL_471(chan->getName()), 1);
		return (false);
	}

	return (true);
}

void	Server::_sendJoinRpls(Client* client, Chan* chan)
{
	std::string				clientsNamesInChan("");
	std::string				joinResponse("");

	joinResponse.assign(':' + client->getFullId() + " JOIN :" + chan->getName() + "\r\n");

	chan->sendToChan(client, joinResponse, 3);

	if (chan->getTopic().size() > 0)
		this->_sendToClient(client->getFd(), "332", RPL_TOPIC_332(chan->getName(), chan->getTopic()), 1);
	this->_sendNamesRpl(chan, client);
	this->_botIntro(chan);
}
