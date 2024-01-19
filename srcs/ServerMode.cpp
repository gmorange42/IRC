#include "../inc/Server.hpp"

void	Server::_modeCmd(int fd, Msg* msg)
{
	if (msg->getParams().empty())
	{
		this->_sendToClient(fd, "461", ERR_NEEDMOREPARAMS_461(msg->getCmd()), 1);
		return ;
	}

	std::string	cpy = msg->getParams();
	std::string	target = "";
	
	split(target, cpy, " ");
	if (target.empty())
		target = cpy;
	if (target[0] == '#')
		this->_chanMode(fd, msg);
	else
		this->_userMode(fd, msg);
}
