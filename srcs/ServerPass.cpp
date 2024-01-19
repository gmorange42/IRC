#include "../inc/Server.hpp"

void	Server::_passCmd(int fd, Msg* msg)
{
	if (msg->getParams().empty())
		this->_sendToClient(fd, "461", ERR_NEEDMOREPARAMS_461(msg->getCmd()), 1);
	else if (this->_clients[fd]->getAuth() == true)
		this->_sendToClient(fd, "462", ERR_ALREADYREGISTRED_462, 1);
	else
		this->_clients[fd]->setPass(msg->getParams());
}

void	Server::_checkPass(int fd, Msg* msg)
{
	std::string	reply;

	reply = "ERROR :Closing link: (" + this->_clients[fd]->getFullId() + ") [Access denied]";
	if (this->_clients[fd]->getPass().empty())
	{
		this->_sendToClient(fd, "", reply, 0);
		this->_delClient(fd);
	}
	else if (this->_pass.compare(this->_clients[fd]->getPass()) == 0)
	{
		this->_clients[fd]->setAuth(true);
		this->_welcome(fd, msg);
	}
	else
	{
		this->_sendToClient(fd, "464", ERR_PASSWDMISMATCH_464, 1);
		this->_sendToClient(fd, "", reply, 0);
		this->_delClient(fd);
	}
}

void	Server::_welcome(int fd, Msg* msg)
{
	std::string	creation = ctime(&this->_time);
	std::string	myInfo = "";
	std::string	info = "";

	creation = creation.substr(0, creation.size() - 1);
	myInfo = myInfo + this->_serverName + " " + this->_version + " " + USERMODES + " " + CHANMODES;
	info = info + "CHANMODES=" + CHANMODES + " CHANNELEN=" + CHANNELEN + " CHANTYPES=" + CHANTYPES + " MAXCHANNELS=" + MAXCHAN + " MODES=" + MODES + " NICKLEN=" + NICKLEN + " PREFIX=" + PREFIX + " USERLEN=" + USERLEN;

	this->_sendToClient(fd, "001", RPL_WELCOME_001(this->_clients[fd]->getFullId()), 1);
	this->_sendToClient(fd, "002", RPL_YOURHOST_002(this->_serverName, this->_version), 1);
	this->_sendToClient(fd, "003", RPL_CREATED_003(creation), 1);
	this->_sendToClient(fd, "004", RPL_MYINFO_004(myInfo), 1);
	this->_sendToClient(fd, "005", RPL_ISUPPORT_005(info), 1);
	msg->getParams().clear();
	this->_motdCmd(fd, msg);
}
