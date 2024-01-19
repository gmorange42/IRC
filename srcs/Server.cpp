#include "../inc/Server.hpp"

Server::Server(int port, std::string pass): _port(port), _pass(pass)
{
	this->_time = time(0);
	this->_serverName = SERVER_NAME;
	this->_version = VERSION;
	this->_sfd = 0;
	bzero(&this->_saddr, sizeof(struct sockaddr_in));
	FD_ZERO(&this->_fds);
	this->_fdMax = 0;
	this->_initCmds();
	this->_clients.clear();
	this->_recv.clear();
	this->_msg.clear();
	this->_chans.clear();
}

Server::~Server(void)
{
}

int	Server::getPort(void) const
{
	return (this->_port);
}

std::string	Server::getPass(void) const
{
	return (this->_pass);
}

std::map<int, Client*>	Server::getClients(void)
{
	return (this->_clients);
}

std::map<std::string, Chan*>	Server::getChans(void)
{
	return (this->_chans);
}

void	Server::_initCmds(void)
{
	this->_cmds.clear();
	this->_cmds["CAP"] = &Server::_capCmd;
	this->_cmds["PASS"] = &Server::_passCmd;
	this->_cmds["NICK"] = &Server::_nickCmd;
	this->_cmds["USER"] = &Server::_userCmd;
	this->_cmds["MOTD"] = &Server::_motdCmd;
	this->_cmds["PRIVMSG"] = &Server::_privmsgCmd;
	this->_cmds["NOTICE"] = &Server::_noticeCmd;
	this->_cmds["QUIT"] = &Server::_quitCmd;
	this->_cmds["JOIN"] = &Server::_joinCmd;
	this->_cmds["PART"] = &Server::_partCmd;
	this->_cmds["MODE"] = &Server::_modeCmd;
	this->_cmds["LIST"] = &Server::_listCmd;
	this->_cmds["PING"] = &Server::_pingCmd;
	this->_cmds["KICK"] = &Server::_kickCmd;
	this->_cmds["TOPIC"] = &Server::_topicCmd;
	this->_cmds["INVITE"] = &Server::_inviteCmd;
	this->_cmds["NAMES"] = &Server::_namesCmd;
}

void	Server::init(void)
{
	socklen_t	val = 1;

	std::cout << "Starting server..." << std::endl;
//	socket()
	this->_sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sfd == -1)
		throw Server::SocketFailedException();

//	setsockopt()
	if (setsockopt(this->_sfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(socklen_t)) == -1)
		throw Server::SetsockoptFailedException();
	
	this->_saddr.sin_family = AF_INET;
	this->_saddr.sin_port = htons(this->_port);
	this->_saddr.sin_addr.s_addr = INADDR_ANY;

//	bind()
	if (bind(this->_sfd, reinterpret_cast<struct sockaddr*>(&this->_saddr), sizeof(struct sockaddr)) == -1)
		throw Server::BindFailedException();

//	listen()
	if (listen(this->_sfd, BACKLOG) == -1)
		throw Server::ListenFailedException();

	FD_ZERO(&this->_fds);
	FD_SET(this->_sfd, &this->_fds);
	this->_fdMax = this->_sfd;
}

void	Server::run(void)
{
	std::cout << "Server running..." << std::endl;
	running = true;
	while (running)
	{
		fd_set	copy = this->_fds;
//		select()
		if (select(this->_fdMax + 1, &copy, NULL, NULL, NULL) == -1 && running == true)
				throw Server::SelectFailedException();
		if (running == false)
			break ;
		for (int i = 0; i <= this->_fdMax; i++)
		{
			if (FD_ISSET(i, &copy))
			{
				if (i == this->_sfd)
					this->_newCo();
				else
					this->_handleInput(i);
			}
		}
	}
	this->_clearAll();
}

void	Server::_newCo(void)
{
	Client*		client = new Client;
	int		fd = 0;
	socklen_t	len = sizeof(struct sockaddr);

//	accept()
	fd = accept(this->_sfd, reinterpret_cast<struct sockaddr*>(&client->getAddr()), &len);
	if (fd == -1)
		throw Server::AcceptFailedException();
	client->setFd(fd);
	FD_SET(fd, &this->_fds);
	if (fd > this->_fdMax)
		this->_fdMax = fd;
	this->_clients.insert(std::make_pair(fd, client));
	this->_recv.insert(std::make_pair(fd, ""));
}

void	Server::_handleInput(int fd)
{
	char		buf[BUF_SIZE];
	ssize_t		nBytes = 0;

	bzero(&buf, BUF_SIZE);
	nBytes = recv(fd, buf, BUF_SIZE, 0);
	if (nBytes == -1)
		throw Server::RecvFailedException();
	if (nBytes == 0)
	{
		this->_sendToChan(fd, "QUIT", ":Connection closed");
		this->_delClient(fd);
	}
	else
	{

		this->_recv[fd].append(buf);
#ifdef DEBUG
		std::cout << "buf: " << buf;
		std::cout << "recv from fd " << fd << ": " << this->_recv[fd];
#endif
		while (this->_getCmd(fd) == true)
		{
#ifdef DEBUG
			std::cout << "msg input: " << this->_msg[fd]->getInput() << std::endl;
#endif
			this->_handleCmd(fd);
		}
	}
}

bool	Server::_getCmd(int fd)
{
	std::string	cmd;

	split(cmd, this->_recv[fd], "\r\n");
	if (!cmd.empty())
	{
		Msg*	msg = new Msg(cmd);
		this->_msg.insert(std::make_pair(fd, msg));
		return (true);
	}
	return (false);
}

void	Server::_handleCmd(int fd)
{
	this->_splitMsg(this->_msg[fd]);
	this->_execCmd(fd, this->_msg[fd]);
	delete this->_msg[fd];
	this->_msg.erase(fd);
}

void	Server::_splitMsg(Msg* msg)
{
	std::string	tmp;
	size_t	i = 0;
	size_t	j = 0;

	while (i < msg->getInput().size())
	{
		while (i < msg->getInput().size() && isspace(msg->getInput()[i]))
			i++;
		j = i;
		while (j < msg->getInput().size() && !isspace(msg->getInput()[j]))
			j++;
		tmp = msg->getInput().substr(i, j - i);
		if (tmp[0] == ':' && msg->getCmd().empty())
			msg->setPrefix(tmp);
		else if (msg->getCmd().empty())
			msg->setCmd(tmp);
		else
		{
			tmp = msg->getInput().substr(i, msg->getInput().size() - i);
			msg->setParams(tmp);
		}
		if (!msg->getParams().empty())
			break ;
		i = j;
	}
#ifdef DEBUG
	std::cout << "msg prefix: " << msg->getPrefix() << std::endl;
	std::cout << "msg cmd: " << msg->getCmd() << std::endl;
	std::cout << "msg params: " << msg->getParams() << std::endl;
#endif
}

void	Server::_execCmd(int fd, Msg* msg)
{
	std::map<std::string, f>::iterator	it;
	std::transform(msg->getCmd().begin(), msg->getCmd().end(), msg->getCmd().begin(), toupper);
	it = this->_cmds.find(msg->getCmd());
	if (it != this->_cmds.end())
		(this->*_cmds[it->first])(fd, msg);
	else
		this->_sendToClient(fd, "421", ERR_UNKNOWNCOMMAND_421(msg->getCmd()), 1);
}

void	Server::_capCmd(int fd, Msg* msg)
{	
	(void)fd;
	(void)msg;
}

bool	Server::_checkAuth(int fd)
{
	if (this->_clients[fd]->getAuth())
		return (true);
	this->_sendToClient(fd, "451", ERR_NOTREGISTERED_451, 1);
	return (false);
}

void    Server::_sendToClient(int fd, std::string cmd, std::string msg, int src)
{
	std::string	reply = "";

	if (src == 1)
	{
		reply = reply + ":" + this->_serverName + " " + cmd;
		if (!this->_clients[fd]->getNickname().empty())
			reply = reply + " " + this->_clients[fd]->getNickname();
		else
			reply = reply + " *";
		reply = reply + " " + msg + "\r\n";
	}
	else if (src == 2)
		reply = ":" + this->_clients[fd]->getFullId() + " " + cmd + " " + msg + "\r\n";
	else
		reply = msg + "\r\n";
#ifdef DEBUG
	std::cout << "reply in sendToClient: " << reply << std::endl;
#endif
	if (send(fd, reply.c_str(), reply.size(), MSG_NOSIGNAL) < 0)
		throw Server::SendFailedException();
}

void    Server::_sendToClient(int src, int dst, std::string cmd, std::string msg)
{
	std::string	reply = "";

	reply = ":" + this->_clients[src]->getFullId() + " " + cmd + " " + this->_clients[dst]->getNickname() + " :" + msg + "\r\n";
	if (send(dst, reply.c_str(), reply.size(), MSG_NOSIGNAL) < 0)
		throw Server::SendFailedException();
}

void	Server::_sendToChan(int fd, std::string cmd, std::string msg)
{
	std::string					reply = "";
	std::map<std::string, Chan*>::iterator		itChan;
	std::map<int, Client*>::iterator		itClient;

	reply = reply + ":" + this->_clients[fd]->getFullId() + " " + cmd + " " + msg + "\r\n";
	for (itChan = this->_chans.begin(); itChan != this->_chans.end(); itChan++)
	{
		itClient = itChan->second->getClients()->find(fd);
		if (itClient != itChan->second->getClients()->end())
			itChan->second->sendToChan(this->_clients[fd], reply, 2);
	}
}

void	Server::_sendToChan(int fd, std::string cmd, std::string msg, std::string chan)
{
	std::string					reply = "";
	std::map<std::string, Chan*>::iterator		itChan;
	std::map<int, Client*>::iterator		itClient;

	reply = reply + ":" + this->_clients[fd]->getFullId() + " " + cmd + " " + chan + " :" + msg + "\r\n";
	if (cmd.compare("PRIVMSG") == 0)
		this->_chans[chan]->sendToChan(this->_clients[fd], reply, 2);
	else
		this->_chans[chan]->sendToChan(this->_clients[fd], reply, 3);
}

void	Server::_delClient(int fd)
{
	this->_rmClient(fd);
	if (this->_clients.count(fd))
	{
		delete this->_clients[fd];
		this->_clients.erase(fd);
	}
	if (this->_msg.count(fd))
	{
		delete this->_msg[fd];
		this->_msg.erase(fd);
	}
	if (this->_recv.count(fd))
		this->_recv.erase(fd);
	FD_CLR(fd, &this->_fds);
	close(fd);
}

void	Server::_rmClient(int fd)
{
	std::map<std::string, Chan*>::iterator	itChan;
	std::vector<std::string>		toRm;

	if (!this->_chans.empty())
	{
		for (itChan = this->_chans.begin(); itChan != this->_chans.end(); itChan++)
		{
			if (itChan->second->getClients()->count(fd))
			{
				if (!itChan->second->rmClient(fd))
					toRm.push_back(itChan->first);
			}
		}
		for (size_t i = 0; i < toRm.size(); i++)
		{
			delete this->_chans[toRm[i]];
			this->_chans.erase(toRm[i]);
		}
	}
}

void	Server::_clearAll()
{
	std::map<int, Client*>::iterator	itClient;
	std::map<int, Msg*>::iterator		itMsg;
	std::map<std::string, Chan*>::iterator	itChan;

	std::cout << "Shutting down server..." << std::endl;
	for (itClient = this->_clients.begin(); itClient != this->_clients.end(); itClient++)
		delete itClient->second;
	this->_clients.clear();
	this->_recv.clear();
	for (itMsg = this->_msg.begin(); itMsg != this->_msg.end(); itMsg++)
		delete itMsg->second;
	this->_msg.clear();
	for (itChan = this->_chans.begin(); itChan != this->_chans.end(); itChan++)
		delete itChan->second;
	this->_chans.clear();
	this->_cmds.clear();
	this->_memes.clear();
	for (int i = 0; i <= this->_fdMax; i++)
	{
		if (FD_ISSET(i, &this->_fds))
		{
			FD_CLR(i, &this->_fds);
			close(i);
		}
	}
	FD_CLR(this->_sfd, &this->_fds);
	close(this->_sfd);
	exit(0);
}

const char*	Server::SocketFailedException::what() const throw()
{
	return ("Call to socket() failed");
}

const char*	Server::SetsockoptFailedException::what() const throw()
{
	return ("Call to setsockopt() failed");
}

const char*	Server::BindFailedException::what() const throw()
{
	return ("Call to bind() failed");
}

const char*	Server::ListenFailedException::what() const throw()
{
	return ("Call to listen() failed");
}

const char*	Server::SelectFailedException::what() const throw()
{
	return ("Call to select() failed");
}

const char*	Server::AcceptFailedException::what() const throw()
{
	return ("Call to accept() failed");
}

const char*	Server::RecvFailedException::what() const throw()
{
	return ("Call to recv() failed");
}

const char*	Server::SendFailedException::what() const throw()
{
	return ("Call to send() failed");
}
