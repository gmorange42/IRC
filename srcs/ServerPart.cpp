#include "../inc/Server.hpp"

void	Server::_partCmd(int fd, Msg* msg)
{
	if (!(this->_checkAuth(fd)))
		return ;

	Client*				client = this->_clients[fd];
	std::vector<std::string>	vecNames;
	Chan*				chan = NULL;
	std::string			message("");
	std::string			reponse("");

	this->_splitPartParams(vecNames, message, msg);
	
	if (!(this->_checkNbParams(vecNames, 1, client, msg)))
		return;
	
	for (std::vector<std::string>::iterator itNames = vecNames.begin();
			itNames != vecNames.end(); ++itNames)
	{
		if (!(chan = this->_findChan(*itNames, client)))
			continue ;

		if (!(this->_checkClientInChan(chan, client, client)))
			continue ;

		if (message.empty())
			message.assign(" :" + client->getNickname());

		reponse.assign(':' + client->getFullId() + " PART " + chan->getName() + message + "\r\n");

		chan->sendToChan(client, reponse, 3);

		if (!(chan->rmClient(fd)))
		{
			delete this->_chans[lower(*itNames)];
			this->_chans.erase(lower(*itNames));
		}
	}
}

void	Server::_splitPartParams(std::vector<std::string>& vecNames, std::string& message, Msg* msg)
{
	vecNames = splitInVector(msg->getParams(), " ");

	if (vecNames.size() == 0)
		return ;

	if (vecNames.size() > 1)
		for (std::vector<std::string>::iterator it = vecNames.begin() + 1;
				it != vecNames.end(); ++it)
			message += ' ' + *it;

	if ((!message.empty()) && message[1] != ':')
		message.insert((message.begin() + 1), ':');

	vecNames = splitInVector(vecNames[0], ",");
}
