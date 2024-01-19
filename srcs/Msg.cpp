#include "../inc/Msg.hpp"

Msg::Msg(std::string input): _input(input)
{
	this->_paramArr.clear();
}

Msg::~Msg(void)
{
	this->_paramArr.clear();
}

void	Msg::setInput(std::string input)
{
	this->_input = input;
}

void	Msg::setPrefix(std::string prefix)
{
	this->_prefix = prefix;
}

void	Msg::setCmd(std::string cmd)
{
	this->_cmd = cmd;
}

void	Msg::setParams(std::string params)
{
	this->_params = params;
}

void	Msg::setParamArr(std::vector<std::string> arr)
{
	this->_paramArr.clear();
	this->_paramArr = arr;
}

std::string&	Msg::getInput(void)
{
	return (this->_input);
}

std::string	Msg::getPrefix(void) const
{
	return (this->_prefix);
}

std::string&	Msg::getCmd(void)
{
	return (this->_cmd);
}

std::string&	Msg::getParams(void)
{
	return (this->_params);
}

std::vector<std::string>	Msg::getParamArr(void)
{
	return (this->_paramArr);
}
