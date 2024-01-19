#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <map>
#include <deque>
#include <exception>
#include <algorithm>
#include <fstream>

#include <ctime>
#include <csignal>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "Client.hpp"
#include "Chan.hpp"
#include "Msg.hpp"
#include "utils.hpp"
#include "rpl.hpp"

# define BACKLOG 10
# define BUF_SIZE 1024

# define SERVER_NAME "ircserv"
# define VERSION "ircserv1.0"
# define USERMODES ","
# define CHANMODES "ilnt"
# define CHANNELEN "50"
# define CHANTYPES "#"
# define MAXCHAN "10"
# define MODES "1"
# define NICKLEN "10"
# define PREFIX "(o)@"
# define USERLEN "10"

# define MOTD_FILE "./motd.txt"

class		Client;
class		Chan;
class		Msg;

extern bool	running;

class	Server
{
	public:

	Server(int port, std::string password);
	~Server(void);

	int				getPort(void) const;
	std::string			getPass(void) const;
	std::map<int, Client*>		getClients(void);
	std::map<std::string, Chan*>	getChans(void);

	void				init(void);
	void				run(void);

	class	SocketFailedException: public std::exception
	{
		virtual const char*	what() const throw();
	};
	class	SetsockoptFailedException: public std::exception
	{
		virtual const char*	what() const throw();
	};
	class	BindFailedException: public std::exception
	{
		virtual const char*	what() const throw();
	};
	class	ListenFailedException: public std::exception
	{
		virtual const char*	what() const throw();
	};
	class	SelectFailedException: public std::exception
	{
		virtual const char*	what() const throw();
	};
	class	AcceptFailedException: public std::exception
	{
		virtual const char*	what() const throw();
	};
	class	RecvFailedException: public std::exception
	{
		virtual const char*	what() const throw();
	};
	class	SendFailedException: public std::exception
	{
		virtual const char*	what() const throw();
	};

	private:

//	private functions
	void				_initCmds(void);
	void				_newCo(void);
	void				_handleInput(int fd);
	bool				_getCmd(int fd);
	void				_handleCmd(int fd);
	void				_splitMsg(Msg* msg);
	void				_execCmd(int fd, Msg* msg);
	bool				_checkAuth(int fd);
	void				_sendToClient(int fd, std::string cmd, std::string msg, int src);
	void				_sendToClient(int src, int dst, std::string cmd, std::string msg);
	void				_sendToChan(int fd, std::string cmd, std::string msg);
	void				_sendToChan(int fd, std::string cmd, std::string msg, std::string chan);
	void				_delClient(int fd);
	void				_rmClient(int fd);
	void				_clearAll(void);

//	cap
	void				_capCmd(int fd, Msg* msg);

//	pass
	void				_passCmd(int fd, Msg* msg);
	void				_checkPass(int fd, Msg* msg);
	void				_welcome(int fd, Msg* msg);

//	nick
	void				_nickCmd(int fd, Msg* msg);
	bool				_checkNick(int fd, Msg* msg);

//	user
	void				_userCmd(int fd, Msg* msg);
	bool				_getUserInfo(std::string& params, std::vector<std::string>& info);
	bool				_setUserInfo(Client* client, std::vector<std::string> info);

//	motd
	void				_motdCmd(int fd, Msg* msg);
	bool				_checkMotdParams(int fd, std::string params);
	void				_sendMotd(int fd);

//	privmsg
	void				_privmsgCmd(int fd, Msg* msg);
	bool				_splitParams(int fd, Msg* msg);
	bool				_splitParams(std::string param, std::string chan);
	int				_checkName(int fd, std::string name);
	bool				_findChan(std::string chan);
	int				_findClient(std::string chan);
	void				_privmsgToChan(int fd, Msg* msg, std::string chan,  int dst);

//	notice
	void				_noticeCmd(int fd, Msg* msg);
	void				_noticeToChan(int fd, Msg* msg, std::string chan, int dst);

//	quit
	void				_quitCmd(int fd, Msg* msg);

//	bot
	void				_botIntro(Chan* chan);
	void				_botEntertain(Chan* chan, std::string param);
	void				_genMeme(Chan* chan);

//	join
	void				_joinCmd(int fd, Msg* msg);
	bool				_splitJoinParams(std::vector<std::string>& vecNames, Msg* msg, Client* client);
	bool				_checkJoinErrors(Chan *chan, Client* client);
	void				_sendJoinRpls(Client* client, Chan* chan);

//	part
	void				_partCmd(int fd, Msg* msg);
	void				_splitPartParams(std::vector<std::string>& vecNames, std::string& message, Msg* msg);

//	mode
	void				_modeCmd(int fd, Msg* msg);
	void				_userMode(int fd, Msg* msg);
	void				_viewUserMode(int fd, std::string client, int dst);
	void				_changeUserMode(int fd, std::string client, int dst, std::string mode);
	void				_chanMode(int fd, Msg* msg);
	char				_splitMode(std::string& chanName, std::string& modes, std::vector<std::string>& vecParams, std::string params);
	void				_sendRpl324(int fd, Chan* chan);
	void				_addModesAndParamsToSend(std::string& modesToSend, std::string& paramsToSend, char mode, std::string params);
	void				_HandleMode(Client* client, std::string& modes, std::vector<std::string>& vecParams, Chan* chan, char operation);
	bool				_modeO(Client* client, std::string param, Chan* chan, char operation);

//	list
	void				_listCmd(int fd, Msg* msg);
	void				_sendListReplys(int fd, Chan* chan);
	void				_inviteCmd(int fd, Msg* msg);
	void				_sendInviteError(int error, Msg *msg, std::vector<std::string> vecParams, Client*  sender, Chan* chan);

//	names
	void				_namesCmd(int fd, Msg* msg);
	void				_sendNamesRplClientsNotInAnyChan(Client* client);
	void				_sendNamesRpl(Chan* chan, Client* client);
	void				_splitNamesParams(std::vector<std::string>& vecNames, Msg* msg);

//	topic
	void				_topicCmd(int fd, Msg* msg);
	void				_modifTopic(std::vector<std::string>& vecParams, Chan* chan, Client* client);

//	ping
	void				_pingCmd(int fd, Msg* msg);

//	kick
	void				_kickCmd(int fd, Msg* msg);
	void				_setKickComment(std::vector<std::string>& vec, std::string& comment);
	Client*				_checkKickErrors(Chan* chan, Client* client, std::string& clientName);

	Chan*				_findChan(std::string chanName, Client* client);
	bool				_checkClientInChan(Chan* chan, Client* clientToSend, Client* clientToCheck);
	bool				_checkClientNotInChan(Chan* chan, Client* clientToSend, Client* clientToCheck);
	bool				_checkClientInChan(Chan* chan, Client* client, bool applicant);
	Client*				_findClientInServer(std::string clientName, Client* client);
	bool				_checkClientIsOp(Chan *chan, Client* client);
	bool				_checkNbParams(std::vector<std::string>& vec, size_t size, Client* client, Msg* msg);

//	private attributes
	typedef void					(Server::*f)(int fd, Msg* msg);
	int						_port;
	std::string					_pass;
	time_t						_time;
	std::string					_serverName;
	std::string					_version;
	int						_sfd;
	struct sockaddr_in				_saddr;
	fd_set						_fds;
	int						_fdMax;
	std::map<int, Client*>				_clients;
	std::map<int, std::string>			_recv;
	std::map<int, Msg*>				_msg;
	std::map<std::string, Chan*>			_chans;
	std::map<std::string, f>			_cmds;
	std::map<int, std::vector<std::string> >	_memes;
};

#endif
