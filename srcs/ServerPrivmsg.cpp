#include "../inc/Server.hpp"

void	Server::_privmsgCmd(int fd, Msg* msg)
{
#ifdef DEBUG
	std::cout << "--- in privmsgCmd ---" << std::endl;
#endif
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
			this->_privmsgToChan(fd, msg, names[i], dst);
			this->_splitParams(msg->getParams(), names[i]);
			continue;
		}

		dst = -1;
		dst = this->_findClient(names[i]);
		if (dst != -1)
			this->_sendToClient(fd, dst, "PRIVMSG", msg->getParams());
		else
			this->_sendToClient(fd, "401", ERR_NOSUCHNICK_401(names[i]), 1);
	}
}

bool	Server::_splitParams(int fd, Msg* msg)
{
	std::string	targets = "";

	if (msg->getParams().empty())
	{
		this->_sendToClient(fd, "461", ERR_NEEDMOREPARAMS_461(msg->getCmd()), 1);
		return (false);
	}
	split(targets, msg->getParams(), " ");
	if (targets.empty())
	{
		this->_sendToClient(fd, "461", ERR_NEEDMOREPARAMS_461(msg->getCmd()), 1);
		return (false);
	}
	msg->setParamArr(splitInVector(targets, ","));
	return (true);
}

bool	Server::_splitParams(std::string param, std::string chan)
{
	std::string	str = "";
	std::string	cpy = param;
	std::string	error = "";

	if (chan[0] == '@')
		chan = lower(chan.substr(1));
	error = error + ":BOT PRIVMSG " + this->_chans[chan]->getName() + " Do you mean \"!BOT surprise me\"?\r\n";
	split(str, cpy, " ");
	if (!str.empty() && str.compare("!BOT") == 0 && cpy.compare("surprise me") == 0)
	{
		this->_botEntertain(this->_chans[chan], param);
		return (true);
	}
	return (false);
}

int	Server::_checkName(int fd, std::string name)
{
	if (name.size() == 1 && name[0] == '@')
	{
		this->_sendToClient(fd, "411", ERR_NORECIPIENT_411, 1);
		return (-1);
	}
	if (name.size() > 1 && name[0] == '@' && name[1] == '#')
	{
		std::string	chan = lower(name.substr(1));
		if (this->_findChan(chan))
			return (0);
	}
	if (name.size() > 1 && name[0] == '#')
	{
		if (this->_findChan(lower(name)))
			return (1);
	}
	return (-2);
}

void	Server::_privmsgToChan(int fd, Msg* msg, std::string chan, int dst)
{
	std::string	reply = "";
	std::string	name = "";

	reply = reply + ":" + this->_clients[fd]->getFullId() + " " + "PRIVMSG" + " ";

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

bool	Server::_findChan(std::string chan)
{
	std::map<std::string, Chan*>::iterator	it;

	if (this->_chans.empty())
		return (false);

	for (it = this->_chans.begin(); it != this->_chans.end(); it++)
	{
		if (it->first == chan)
			return (true);
	}
	return (false);
}

int	Server::_findClient(std::string toFind)
{
	std::map<int, Client*>::iterator	it;
	std::string				name = lower(toFind);

	for (it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (name.compare(lower(it->second->getNickname())) == 0)
			return (it->second->getFd());
	}
	return (-1);
}
