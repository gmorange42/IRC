#include "../inc/Server.hpp"

void	Server::_userMode(int fd, Msg* msg)
{
	std::string	target = "";
	int		dst = -1;

	split(target, msg->getParams(), " ");
	if (target.empty())
	{
		dst = this->_findClient(msg->getParams());
		this->_viewUserMode(fd, msg->getParams(), dst);
		return ;
	}

	std::string	mode = msg->getParams();

	dst = this->_findClient(target);
	this->_changeUserMode(fd, target, dst, mode);
}

void	Server::_viewUserMode(int fd, std::string client, int dst)
{
	std::string	action = "view modes";

	if (dst == fd)
		this->_sendToClient(fd, "221", RPL_UMODEIS_221, 1);
	else if (dst != -1)
		this->_sendToClient(fd, "502", ERR_USERSDONTMATCH_502(action), 1);
	else
		this->_sendToClient(fd, "401", ERR_NOSUCHNICK_401(client), 1);
}

void	Server::_changeUserMode(int fd, std::string client, int dst, std::string mode)
{
	std::string	action = "change mode";

	if (dst == fd && mode.compare("+i") == 0)
		this->_sendToClient(fd, "501", ERR_UMODEUNKNOWNFLAG_501, 1);
	else if (dst != -1 && mode.compare("+i") == 0)
		this->_sendToClient(fd, "502", ERR_USERSDONTMATCH_502(action), 1);
	else if (dst != -1)
		this->_sendToClient(fd, "501", ERR_UMODEUNKNOWNFLAG_501, 1);
	else
		this->_sendToClient(fd, "401", ERR_NOSUCHNICK_401(client), 1);
}
