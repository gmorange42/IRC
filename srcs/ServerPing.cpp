#include "../inc/Server.hpp"

void	Server::_pingCmd(int fd, Msg* msg)
{
	if (!(this->_checkAuth(fd)))
		return ;
	
	if (msg->getParams().size() == 0)
	{
		this->_sendToClient(fd, "461", ERR_NEEDMOREPARAMS_461(msg->getCmd()), 1);
		return ;
	}
	this->_sendToClient(fd, "PONG", ":" + msg->getParams(), 2);
}
