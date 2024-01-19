#ifndef MSG_HPP
#define MSG_HPP

#include <iostream>
#include <string>
#include <vector>

class	Msg
{
	public:

	Msg(std::string input);
	~Msg(void);

	void				setInput(std::string input);
	void				setPrefix(std::string prefix);
	void				setCmd(std::string cmd);
	void				setParams(std::string params);
	void				setParamArr(std::vector<std::string> arr);
	std::string&			getInput(void);
	std::string			getPrefix(void) const;
	std::string&			getCmd(void);
	std::string&			getParams(void);
	std::vector<std::string>	getParamArr(void);

	private:

	std::string			_input;
	std::string			_prefix;
	std::string			_cmd;
	std::string			_params;
	std::vector<std::string>	_paramArr;
//	prefix, cmd and all parameters are separated by one ASCII space character
};

#endif
