#include "../inc/Server.hpp"

void	Server::_botIntro(Chan* chan)
{
	std::string	intro = "";

	intro = ":BOT PRIVMSG " + chan->getName() + " :Welcome! If you're bored, just try \"!BOT surprise me\"\r\n";
	chan->sendToChan(NULL, intro, 3);
}

void	Server::_botEntertain(Chan* chan, std::string param)
{
	std::string	str = "";

	split(str, param, " ");
	this->_genMeme(chan);
	srand(time(0));
	switch (rand() % 4)
	{
		case 0: 
			for (size_t i = 0; i < this->_memes[0].size(); i++)
				chan->sendToChan(NULL, this->_memes[0][i], 3);
			break ;
		case 1: 
			for (size_t i = 0; i < this->_memes[1].size(); i++)
				chan->sendToChan(NULL, this->_memes[1][i], 3);
			break ;
		case 2: 
			for (size_t i = 0; i < this->_memes[2].size(); i++)
				chan->sendToChan(NULL, this->_memes[2][i], 3);
			break ;
		case 3: 
			for (size_t i = 0; i < this->_memes[3].size(); i++)
				chan->sendToChan(NULL, this->_memes[3][i], 3);
			break ;
	}
}
