#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <vector>

void				split(std::string& msg, std::string& toSplit, std::string toFind);
bool				checkChars(std::string toCheck, bool flag);
std::vector<std::string>	splitInVector(std::string str, std::string delimiter);
std::string			lower(std::string);

#endif
