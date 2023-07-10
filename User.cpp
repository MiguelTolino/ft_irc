/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 12:42:01 by mmateo-t          #+#    #+#             */
/*   Updated: 2023/07/10 09:57:18 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User(int fd)
	: _fd(fd), _nick(""), _username(""), _realname(""), _channels(), forbittenChar(",!?*@.&#")
{
	_isLogged = false;
	_operator = false;
	_welcomeFlag = false;
}

User::~User()
{
	for (std::set<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		(*it)->removeUser(this);
	}
	_channels.clear();

	if (close(this->_fd) < 0)
	{
		logg(LOG_ERROR) << "Close() Error\n";
	}
}

int User::getFd() const
{
	return _fd;
}

bool User::setNick(const std::string &nick)
{
	bool isForbbiten = false;

	if (nick.at(0) == '$' || nick.at(0) == ':')
	{
		isForbbiten = true;
		return isForbbiten;
	}
	for (size_t i = 0; i < this->forbittenChar.size(); i++)
	{
		if (nick.find(this->forbittenChar[i]) != std::string::npos)
		{
			isForbbiten = true;
			break;
		}
	}
	if (!isForbbiten)
		// nick limitation
		_nick = nick.substr(0,16);

	return isForbbiten;
}

const std::string &User::getNick() const
{
	return _nick;
}

bool User::setUsername(const std::string &username)
{
	_username = username;
	return true;
}
const std::string &User::getUsername() const
{
	return _username;
}

bool User::isLogged() const
{
	return _isLogged;
}

bool User::isOperator() const
{
	return _operator;
}

void User::changeToLogged()
{
	_isLogged = true;
}

void User::changeToOperator()
{
	_operator = true;
	logg(LOG_INFO) << ORANGE << this->_nick << RESET << " changed to operator\n";
}

void User::addChannel(Channel *channel)
{
	_channels.insert(channel);
}

void User::removeChannel(Channel *channel)
{
	_channels.erase(channel);
}

std::string User::getRealName() const
{
	return this->_realname;
}

void User::setRealName(const std::string &user)
{
	if (this->_realname.empty())
	{
		if (user.at(0) == ':')
		{
			this->_realname = user.substr(1);
		}
		else
		{
			this->_realname = user;
		}
	}
	else
	{
		this->_realname += (" " + user);
	}
}

void User::removeAllChannels()
{
	this->_channels.clear();
}

bool User::isWelcomeSent() const
{
	return _welcomeFlag;
}

void User::setWelcomeFlag()
{
	_welcomeFlag = true;
}
