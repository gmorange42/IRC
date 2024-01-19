#include "../inc/Client.hpp"

Client::Client(void)
{
#ifdef DEBUG
	std::cout << "Client constructor called" << std::endl;
#endif
	this->_cfd = 0;
	this->_auth = false;
	bzero(&this->_caddr, sizeof(struct sockaddr_in));
}

Client::~Client(void)
{
#ifdef DEBUG
	std::cout << "Client destructor called" << std::endl;
#endif
}

void	Client::setFd(int fd)
{
	this->_cfd = fd;
}

void	Client::setAuth(bool val)
{
	this->_auth = val;
}

void	Client::setPass(std::string pass)
{
	this->_pass = pass;
}

void	Client::setNickname(std::string nickname)
{
	this->_nickname = nickname;
}

void	Client::setUsername(std::string username)
{
	this->_username = username;
}

void	Client::setHostname(std::string hostname)
{
	this->_hostname = hostname;
}

void	Client::setServername(std::string servername)
{
	this->_servername = servername;
}

void	Client::setRealname(std::string realname)
{
	this->_realname = realname;
}

void	Client::setFullId(void)
{
	this->_fullId.clear();
	this->_fullId.append(this->_nickname);
	this->_fullId.append("!");
	this->_fullId.append(this->_username);
	this->_fullId.append("@");
	this->_fullId.append(this->_hostname);
}

int	Client::getFd(void) const
{
	return (this->_cfd);
}

struct sockaddr_in&	Client::getAddr(void)
{
	return (this->_caddr);
}

bool	Client::getAuth(void) const
{
	return (this->_auth);
}

std::string	Client::getPass(void) const
{
	return (this->_pass);
}

std::string	Client::getNickname(void) const
{
	return (this->_nickname);
}

std::string	Client::getUsername(void) const
{
	return (this->_username);
}

std::string	Client::getHostname(void) const
{
	return (this->_hostname);
}

std::string	Client::getServername(void) const
{
	return (this->_servername);
}

std::string	Client::getFullId(void) const
{
	return (this->_fullId);
}

bool	Client::operator!=(Client const& c)
{
	return (this->_cfd != c.getFd());
}
