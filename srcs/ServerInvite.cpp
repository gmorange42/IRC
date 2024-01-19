#include "../inc/Server.hpp"

void	Server::_inviteCmd(int fd, Msg* msg)
{
	if (!(this->_checkAuth(fd)))
		return ;

	Client*					sender = this->_clients[fd];
	Client*					invite;
	std::vector<std::string>		vecParams;
	Chan*					chan;

	vecParams = splitInVector(msg->getParams(), " ");

	if (!(this->_checkNbParams(vecParams, 2, sender, msg)))
		return ;

	if (!(chan = this->_findChan(vecParams[1], sender)))
		return ;

	if (!(invite = this->_findClientInServer(vecParams[0], sender)))
		return ;

	if (!(this->_checkClientNotInChan(chan, sender, invite)))
		return ;

	if (!(this->_checkClientInChan(chan, sender, sender)))
		return ;

	if (!(this->_checkClientIsOp(chan, sender)))
		return;

	if (chan->inviteClient(invite))
		this->_sendToClient(fd, invite->getFd(), msg->getCmd(), chan->getName());
}
