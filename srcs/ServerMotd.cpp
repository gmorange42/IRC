#include "../inc/Server.hpp"

void	Server::_motdCmd(int fd, Msg* msg)
{
	if (this->_clients[fd]->getAuth() == false)
	{
		this->_sendToClient(fd, "451", ERR_NOTREGISTERED_451, 1);
		return ;
	}
	if (!msg->getParams().empty())
	{
		if (this->_checkMotdParams(fd, msg->getParams()) == false)
			return ;
	}
	this->_sendMotd(fd);
}

bool	Server::_checkMotdParams(int fd, std::string params)
{
	std::string	name = "";

	split(name, params, " ");
	if (name.empty())
		name = params;
	if (name[0] == ':')
		name = name.substr(1);
	if (this->_serverName.compare(name) == 0)
		return (true);
	this->_sendToClient(fd, "402", ERR_NOSUCHSERVER_402(name), 1);
	return (false);
}

void	Server::_sendMotd(int fd)
{
	std::ifstream	infile;
	std::string	line;

	infile.open(MOTD_FILE);
	if (!infile.is_open())
	{
		this->_sendToClient(fd, "422", ERR_NOMOTD_422, 1);
		return ;
	}
	this->_sendToClient(fd, "375", RPL_MOTDSTART_375(this->_serverName), 1);
	while (getline(infile, line))
		this->_sendToClient(fd, "372", RPL_MOTD_372(line), 1);
	this->_sendToClient(fd, "376", RPL_ENDOFMOTD_376, 1);
	infile.close();
}
