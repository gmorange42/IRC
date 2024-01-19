#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <map>
#include <exception>

#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "Chan.hpp"

class	Chan;

class	Client
{
	public:

	Client(void);
	~Client(void);

	void	setFd(int fd);
	void	setAuth(bool val);
	void	setPass(std::string pass);
	void	setNickname(std::string nickname);
	void	setUsername(std::string username);
	void	setHostname(std::string hostname);
	void	setServername(std::string servername);
	void	setRealname(std::string realname);
	void	setFullId(void);

	int					getFd(void) const;
	struct sockaddr_in&			getAddr(void);
	bool					getAuth(void) const;
	std::string				getPass(void) const;
	std::string				getNickname(void) const;
	std::string				getUsername(void) const;
	std::string				getHostname(void) const;
	std::string				getServername(void) const;
	std::string				getRealname(void) const;
	std::string				getFullId(void) const;

	bool	operator!=(Client const& c);

	private:

	int			_cfd;
	struct sockaddr_in	_caddr;
	bool			_auth;
	std::string		_pass;
	std::string		_nickname;
	std::string		_username;
	std::string		_hostname;
	std::string		_servername;
	std::string		_realname;
	std::string		_fullId;
};

#endif
