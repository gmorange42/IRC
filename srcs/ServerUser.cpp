#include "../inc/Server.hpp"

void	Server::_userCmd(int fd, Msg* msg)
{
	std::vector<std::string>	info;
	Client*				client = this->_clients[fd];

	if (client->getAuth())
		this->_sendToClient(fd, "462", ERR_ALREADYREGISTRED_462, 1);
	else
	{
		if (this->_getUserInfo(msg->getParams(), info) == true)
		{
			if (this->_setUserInfo(client, info) == true)
			{
				if (!client->getNickname().empty() && !client->getUsername().empty())
					this->_checkPass(fd, msg);
			}
		}
		else
			this->_sendToClient(fd, "461", ERR_NEEDMOREPARAMS_461(msg->getCmd()), 1);
	}
}

bool	Server::_getUserInfo(std::string& params, std::vector<std::string>& info)
{
	std::string			tmp;
	std::string			lastParam;
	size_t				pos;

	pos = params.find(":");
	if (pos != std::string::npos)
	{
		lastParam = params.substr(pos);
		params = params.substr(0, pos);
	}

	params = params + " ";
	while (!params.empty())
	{
		tmp.clear();
		split(tmp, params, " ");
		if (!tmp.empty())
			info.push_back(tmp);
	}
	if (!lastParam.empty())
		info.push_back(lastParam);
	if (info.size() < 4)
		return (false);
	return (true);
}

bool	Server::_setUserInfo(Client* client, std::vector<std::string> info)
{
	char*		str = NULL;
	std::string	reply = "";

	str = inet_ntoa(client->getAddr().sin_addr);
	client->setHostname(str);
	reply = "ERROR :Closing link: (" + client->getHostname() + ") [Invalid username]";

	if (!checkChars(info[0], false))
	{
		this->_sendToClient(client->getFd(), "", reply, 0);
		this->_delClient(client->getFd());
		return (false);
	}

	if (info[0].size() > static_cast<unsigned long>(atoi(USERLEN)))
		info[0] = info[0].substr(0, static_cast<unsigned long>(atoi(USERLEN)));

	client->setUsername(info[0]);
	client->setServername(info[2]);
	client->setRealname(info[3]);
	client->setFullId();
	return (true);
}
