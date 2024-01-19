#include "../inc/Server.hpp"

void	Server::_kickCmd(int fd, Msg* msg)
{
	if (!(this->_checkAuth(fd)))
		return ;

	Client*				client = this->_clients[fd];
	Client*				clientToKick;
	Chan*				chan;
	std::vector<std::string>	vecUsers;
	std::vector<std::string>	vecChans;
	std::string			comment("");
	std::string			reponse("");		

	vecUsers = splitInVector(msg->getParams(), " ");

	if (!(this->_checkNbParams(vecUsers, 2, client, msg)))
		return ;

	this->_setKickComment(vecUsers, comment);
	vecChans = splitInVector(vecUsers[0], ",");
	vecUsers = splitInVector(vecUsers[1], ",");

	for (std::vector<std::string>::iterator itChan = vecChans.begin();
			itChan != vecChans.end(); ++itChan)
	{
		chan = NULL;
		clientToKick = NULL;
		if (!(chan = this->_findChan(*itChan, client)))
			continue ;

		for (std::vector<std::string>::iterator itUser = vecUsers.begin();
				itUser != vecUsers.end(); ++itUser)
		{
			if (!(clientToKick = this->_checkKickErrors(chan, client, *itUser)))
				continue ;
			if (comment.empty())
				comment.assign(client->getNickname());
			reponse.assign(':' + client->getFullId() + " KICK " + chan->getName() + ' ' + clientToKick->getNickname() + " :" + comment + "\r\n");
			chan->sendToChan(client, reponse, 3);
			if (!(chan->rmClient(clientToKick->getFd())))
			{
				delete this->_chans[lower(*itChan)];
				this->_chans.erase(lower(*itChan));
			}
		}
	}
}

void	Server::_setKickComment(std::vector<std::string>& vec, std::string& comment)
{
	if (vec.size() > 2)
	{
		if (vec[2][0] == ':')
			vec[2].erase(vec[2].begin());
		for (std::vector<std::string>::iterator it = (vec.begin() + 2);
				it != vec.end(); ++it)
		{
			if (it != (vec.begin() + 2))
				comment += " ";
			comment += *it;
		}
	}
}

Client*	Server::_checkKickErrors(Chan* chan, Client* client, std::string& clientName)
{
	Client	*clientToKick = NULL;

	if (!(clientToKick = this->_findClientInServer(clientName, client)))
		return(NULL);

	if (!(this->_checkClientInChan(chan, client, clientToKick)))
		return(NULL);

	if (!(this->_checkClientInChan(chan, client, client)))
		return(NULL);

	if (!(this->_checkClientIsOp(chan, client)))
		return(NULL);
	return (clientToKick);
}
