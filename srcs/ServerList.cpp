#include "../inc/Server.hpp"

void	Server::_listCmd(int fd, Msg* msg)
{
	if (!(this->_checkAuth(fd)))
		return ;

	this->_sendToClient(fd, "321", RPL_LISTSTART_321, 1);

	if ((msg->getParams().size()) == 0)
	{
		for (std::map<std::string, Chan*>::iterator it = this->_chans.begin();
				it != this->_chans.end(); ++it)
			this->_sendListReplys(fd, it->second);
	}
	else
	{
		std::vector<std::string>		vecChans;
		std::map<std::string, Chan*>::iterator	itChan;

		vecChans = splitInVector(msg->getParams(), " ");
		vecChans = splitInVector(vecChans[0], ",");

		for (std::vector<std::string>::iterator it = vecChans.begin();
				it != vecChans.end(); ++it)
		{
			itChan = this->_chans.find(*it);
			if (itChan != this->_chans.end())
				this->_sendListReplys(fd, itChan->second);
		}
	}

	this->_sendToClient(fd, "323", RPL_LISTEND_323, 1);
}

void	Server::_sendListReplys(int fd, Chan* chan)
{
	std::stringstream nbUsers("");

	nbUsers << chan->getClients()->size();
	this->_sendToClient(fd, "322", RPL_LIST_322(chan->getName(), nbUsers.str(), chan->getTopic()), 1);
}
