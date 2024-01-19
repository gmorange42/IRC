#include "../inc/Server.hpp"

void	Server::_topicCmd(int fd, Msg* msg)
{
	if (!(this->_checkAuth(fd)))
		return ;

	std::vector<std::string>	vecParams;
	Client*				client = this->_clients[fd];
	Chan*				chan = NULL;

	vecParams = splitInVector(msg->getParams(), " ");

	if (!(this->_checkNbParams(vecParams, 1, client, msg)))
		return ;

	if (!(chan = this->_findChan(vecParams[0], client)))
		return ;

	if (vecParams.size() == 1)
	{
		if (chan->getTopic().size() > 0)
			this->_sendToClient(client->getFd(), "332", RPL_TOPIC_332(chan->getName(), chan->getTopic()), 1);
		else
			this->_sendToClient(client->getFd(), "331", RPL_NOTOPIC_331(chan->getName()), 1);
	}
	else
		this->_modifTopic(vecParams, chan, client);
}

void	Server::_modifTopic(std::vector<std::string>& vecParams, Chan* chan, Client* client)
{
	std::string			newTopic("");

	if (!(this->_checkClientInChan(chan, client, client)))
		return ;

	if (chan->checkMode('t'))
	{
		if (!(_checkClientIsOp(chan, client)))
			return ;
	}

	for (std::vector<std::string>::iterator it = vecParams.begin() + 1;
		it != vecParams.end(); ++it)
	{
		if (it != vecParams.begin() + 1)
			newTopic += " ";
		newTopic += *it;
	}

	if (newTopic[0] == ':')
		newTopic.erase(newTopic.begin());

	chan->setTopic(newTopic);
	this->_sendToChan(client->getFd(), "TOPIC", newTopic, chan->getName());
}
