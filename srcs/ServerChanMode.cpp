#include "../inc/Server.hpp"

void	Server::_chanMode(int fd, Msg* msg)
{
	if (!(this->_checkAuth(fd)))
		return ;

	Client*				client = this->_clients[fd];
	std::string			modes("");
	std::vector<std::string>	vecParams;
	char				operation;
	std::string			chanName("");
	std::map<std::string, Chan*>::iterator	itChan;
	Chan*				chan = NULL;

	if ((operation = this->_splitMode(chanName, modes, vecParams, msg->getParams())) == 'x')
	{
		this->_sendToClient(fd, "461", ERR_NEEDMOREPARAMS_461(msg->getCmd()), 1);
		return ;
	}

	if (chanName[0] != '#')
		return ;

	if (!(chan = this->_findChan(chanName, client)))
		return;

	if (modes.size() == 1 && modes.compare("b") == 0)
        {
                this->_sendToClient(fd, "368", RPL_ENDOFBANLIST_368(chan->getName()), 1);
                return ;
        }

	if (modes.size() == 0)
	{
		this->_sendToClient(fd, "324", RPL_CHANNELMODEIS_324(chan->getName(), '+' + chan->getModes()), 1);
		return ;
	}

	if (!this->_checkClientIsOp(chan, client))
		return;

	this->_HandleMode(client, modes, vecParams, chan, operation);
}

void	Server::_addModesAndParamsToSend(std::string& modesToSend, std::string& paramsToSend, char mode, std::string params)
{
	modesToSend += mode;
	if (params.size() > 0)
		paramsToSend += (" " + params);
}

void	Server::_HandleMode(Client* client, std::string& modes, std::vector<std::string>& vecParams, Chan* chan, char operation)
{
	std::vector<std::string>::iterator	itParams;
	std::string				modesToSend("");
	std::string				paramsToSend("");
	std::string				response("");
	std::string				temp;

	modesToSend = operation;
	itParams = vecParams.begin();
	for (std::string::iterator it = modes.begin(); it != modes.end(); ++it)
	{
		temp = "";
		if (*it == 'o')
		{
			if (itParams == vecParams.end())
				continue ;
			if (this->_modeO(client, *itParams, chan, operation))
				this->_addModesAndParamsToSend(modesToSend, paramsToSend, 'o', *itParams);
			++itParams;
		}
		else if (*it == 'l')
		{
			if (itParams != vecParams.end() && operation == '+')
				temp = *itParams;
			if (chan->changeLimite(temp, operation))
			{
				this->_addModesAndParamsToSend(modesToSend, paramsToSend, 'l', temp);
				if (operation == '+')
					++itParams;
			}
		}
		else if (*it == 'i' || *it == 't' || *it == 'n')
		{
			if (chan->changeMode(operation, static_cast<char>(*it)))
				this->_addModesAndParamsToSend(modesToSend, paramsToSend, *it, "");
		}
		else
			this->_sendToClient(client->getFd(), "472", ERR_UNKNOWNMODE_472((temp = *it), chan->getName()), 1);
	}
	if (modesToSend.size() > 1)
	{
		response.assign(':' + client->getFullId() + " MODE " + chan->getName() + ' ' + modesToSend + paramsToSend + "\r\n");
		chan->sendToChan(client, response, 3);
	}
}

bool	Server::_modeO(Client* client, std::string param, Chan* chan, char operation)
{
	std::map<int, Client*>::iterator	itNewOp;
	Client*					newOp = NULL;

	if (!(newOp = this->_findClientInServer(param, client)))
		return (false);
	if (chan->isInChan(newOp->getFd()))
	{
		if (chan->modifOper(newOp, operation))
			return (true);
	}
	return (false);
}

char	Server::_splitMode(std::string& chanName, std::string& modes, std::vector<std::string>& vecParams, std::string params)
{
	char	operation;
	std::string::iterator	itParams;

	vecParams = splitInVector(params, " ");

	if (vecParams.size() < 1)
		return ('x');

	chanName = vecParams[0];

	if (vecParams.size() < 2)
		return ('s');
	modes = vecParams[1];

	if (*modes.begin() == '+' || *modes.begin() == '-')
	{
		operation = *modes.begin();
		modes.erase(modes.begin());
	}
	else
		operation = '+';

	vecParams.erase(vecParams.begin(), (vecParams.begin() + 2));

	return (operation);
}	
