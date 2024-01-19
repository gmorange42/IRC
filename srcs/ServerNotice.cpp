#include "../inc/Server.hpp"

void	Server::_noticeCmd(int fd, Msg* msg)
{
	if (!this->_checkAuth(fd))
		return ;
	if (!this->_splitParams(fd, msg))
		return ;

	std::vector<std::string>	names = msg->getParamArr();

	if (msg->getParams()[0] == ':')
		msg->getParams() = msg->getParams().substr(1);

	for (size_t i = 0; i < names.size(); i++)
	{
		int	dst = -2;

		dst = this->_checkName(fd, names[i]);
		if (dst == -1)
			break ;

		if (dst == 0 || dst == 1)
		{
			this->_noticeToChan(fd, msg, names[i], dst);
			continue;
		}

		dst = -1;
		dst = this->_findClient(names[i]);

		if (dst != -1)
			this->_sendToClient(fd, dst, "NOTICE", msg->getParams());
		else
			this->_sendToClient(fd, "401", ERR_NOSUCHNICK_401(names[i]), 1);
	}
}

void	Server::_noticeToChan(int fd, Msg* msg, std::string chan, int dst)
{
	std::string	reply = "";
	std::string	name = "";

	reply = reply + ":" + this->_clients[fd]->getFullId() + " " + "NOTICE" + " ";

	if (dst == 0)
		name = lower(chan.substr(1));
	else
		name = lower(chan);

	if (this->_chans[name]->checkMode('n') && !this->_chans[name]->isInChan(fd))
		this->_sendToClient(fd, "404", ERR_CANNOTSENDTOCHAN_404(chan), 1);
	else if (dst == 0)
	{
		reply = reply + "@" + this->_chans[name]->getName() + " :" + msg->getParams() + "\r\n";
		this->_chans[name]->sendToChan(this->_clients[fd], reply, 1);
	}
	else if (dst == 1)
	{
		reply = reply + this->_chans[name]->getName() + " :" + msg->getParams() + "\r\n";
		this->_chans[name]->sendToChan(this->_clients[fd], reply, 2);
	}
}
