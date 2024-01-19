#include "../inc/Server.hpp"

void	Server::_quitCmd(int fd, Msg* msg)
{
	std::string	reply;

	reply = "ERROR :Closing link: (" + this->_clients[fd]->getFullId() + ") [QUIT: ";

	if (!msg->getParams().empty())
	{
		if (msg->getParams()[0] == ':')
			msg->setParams(msg->getParams().substr(1));
		reply = reply + msg->getParams() + "]";
	}
	else
		reply = reply + this->_clients[fd]->getNickname() + "]";
	this->_sendToClient(fd, "", reply, 0);

	reply.clear();
	if (!msg->getParams().empty())
		reply = reply + ":Quit: " + msg->getParams();
	else
		reply = reply + ":Quit: " + this->_clients[fd]->getNickname();

	this->_sendToChan(fd, "QUIT", reply);
	this->_delClient(fd);
}
