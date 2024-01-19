#include "../inc/utils.hpp"
#include "../inc/Server.hpp"
#include "../inc/Msg.hpp"

void	split(std::string& msg, std::string& toSplit, std::string toFind)
{
	size_t		pos = 0;

	pos = toSplit.find(toFind);
	if (pos != std::string::npos)
	{
		msg = toSplit.substr(0, pos);
		toSplit = toSplit.substr(pos + toFind.size());
	}
}

bool	checkChars(std::string toCheck, bool flag)
{
	if (flag == true && isdigit(toCheck[0]))
		return (false);
	for (size_t i = 0; i < toCheck.size(); i++)
	{
		if (!isalnum(toCheck[i]) && toCheck[i] != '_')
			return (false);
	}
	return (true);
}

std::vector<std::string>	splitInVector(std::string str, std::string delimiter)
{
	size_t				pos;
	std::vector<std::string>	vec;

	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		vec.push_back(str.substr(0, pos));
		str.erase(0, pos + delimiter.size());
	}
	vec.push_back(str.substr());
	if (vec.size() == 1 && vec[0].empty())
		vec.clear();
	return (vec);
}

std::string	lower(std::string str)
{
	std::string	ret("");

	ret.resize(str.size());
	std::transform(str.begin(), str.end(), ret.begin(), tolower);

	return (ret);
}
