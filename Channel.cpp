/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 12:43:58 by mmateo-t          #+#    #+#             */
/*   Updated: 2023/07/10 10:20:17 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string &name)
	: _name(), _topic("New channel"), _password(), _users(), _admins(), _invited(), _baned()
{
	_limit = 0;
	_nbrUsers = 0;
	_isPrivate = false;
	_isSecret = false;
	_isInviteOnly = false;
	_isModerate = false;
	_isTopicBlock = true;
	_isBlockOutside = true;

	this->setName(name);
}

Channel::~Channel()
{
}

const std::string &Channel::getName() const
{
	return "#" + _name;
}

bool Channel::setName(const std::string &name)
{
	if (name.find(' ') != std::string::npos || name.find(',') > std::string::npos)
	{
		logg(LOG_WARNING) << "Channel name contains forbidden characters\n";
		return false;
	}
	this->_name = name;
	return true;
}

int Channel::addUser(User *user)
{
	if (this->_users.find(user) != this->_users.end())
	{
		return false;
	}

	if (_users.empty())
	{
		_admins.insert(user);
	}

	_nbrUsers++;
	_users.insert(user);
	user->addChannel(this);
	return true;
}

void Channel::removeUser(User *user)
{
	_users.erase(user);
	_admins.erase(user);
	_invited.erase(user);
	_nbrUsers--;
	logg(LOG_DEBUG) << GRAY << user->getNick() << " removed from the channel (" << this->_name << ")\n" << RESET;
}

void Channel::setTopic(const std::string &topic)
{
	_topic = topic;
	logg(LOG_INFO) << "Channel: " << ORANGE << this->_name << RESET << " | New Topic: " << ORANGE << this->_topic << "\n"
				   << RESET;
}
const std::string &Channel::getTopic() const
{
	return _topic;
}

bool Channel::setMode(std::string mode)
{
	bool add_remove;

	for (size_t i = 0; i < mode.size(); i++)
	{
		if (mode[i] == '+')
		{
			add_remove = true;
		}
		else if (mode[i] == '-')
		{
			add_remove = false;
		}
		else
		{
			switch (mode[i])
			{
			case 'i': // invite-only
				_isInviteOnly = add_remove;
				break;
			case 't': // topic settable by channel operator only flag;
				_isTopicBlock = add_remove;
				break;
			case 'n': // no messages to channel from clients on the outside
				_isBlockOutside = add_remove;
				break;
			default:
				return false;
			}
		}
	}
	return true;
}

	bool Channel::isAdmin(User * admin) const
	{
		return _admins.find(admin) != _admins.end();
	}

	bool Channel::isUser(User * user) const
	{
		return _users.find(user) != _users.end();
	}

	bool Channel::isBaned(User * user) const
	{
		return _baned.find(user) != _baned.end();
	}

	bool Channel::isInvited(User * user) const
	{
		return _invited.find(user) != _invited.end();
	}

	int Channel::addAdmin(User * user)
	{
		_admins.insert(user);
		return 0;
	}

	bool Channel::isEmpty() const
	{
		return _users.empty();
	}

	bool Channel::isPrivate() const
	{
		return _isPrivate;
	}
	bool Channel::isSecret() const
	{
		return _isSecret;
	}

	bool Channel::isInviteOnly() const
	{
		return _isInviteOnly;
	}

	bool Channel::isModerate() const
	{
		return _isModerate;
	}

	bool Channel::isTopicBlock() const
	{
		return _isTopicBlock;
	}

	bool Channel::isBlockOutside() const
	{
		return _isBlockOutside;
	}

	int Channel::setPrivate(bool mode, User *user)
	{
		if (isAdmin(user))
		{
			_isPrivate = mode;
			return true;
		}
		return false;
	}

	int Channel::setSecret(bool mode, User *user)
	{
		if (isAdmin(user))
		{
			_isSecret = mode;
			return true;
		}
		return false;
	}

	int Channel::setInviteOnly(bool mode, User *user)
	{
		if (isAdmin(user))
		{
			_isInviteOnly = mode;
			return true;
		}
		return false;
	}

	int Channel::setModerate(bool mode, User *user)
	{
		if (isAdmin(user))
		{
			_isModerate = mode;
			return true;
		}
		return false;
	}

	int Channel::setTopicBlock(bool mode, User *user)
	{
		if (isAdmin(user))
		{
			_isTopicBlock = mode;
			return true;
		}
		return false;
	}

	int Channel::setBlockOutside(bool mode, User *user)
	{
		if (isAdmin(user))
		{
			_isBlockOutside = mode;
			return true;
		}
		return false;
	}

	std::set<User *> Channel::getUsers() const
	{
		return this->_users;
	}

	std::string Channel::getModes() const
	{
		//: irc.example.com MODE #channel +nt
		std::string mode("+");

		if (_isPrivate)
			mode += std::string("p");
		if (_isSecret)
			mode += std::string("s");
		if (_isInviteOnly)
			mode += std::string("i");
		if (_isTopicBlock)
			mode += std::string("t");
		if (_isBlockOutside)
			mode += std::string("n");
		if (_isModerate)
			mode += std::string("m");
		return mode;
	}

	std::string Channel::getListUsers() const
	{
		std::string users;
		std::set<User *>::iterator it;

		for (it = _users.begin(); it != _users.end(); it++)
		{
			if (!users.empty())
				users += std::string(" ");
			if (isAdmin(*it))
				users += std::string("@");
			users += (*it)->getNick();
		}
		return users + "\n/NAMES list";
	}

	void Channel::inviteUser(User * user)
	{
		this->_invited.insert(user);
	}
