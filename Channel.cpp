#include "Channel.hpp"
Channel::Channel(const std::string& name)
:_name(name), _topic("Nuevo canal"), _password(),_users(), _admins(), _voiced(), _baned()
{
	_limit = 0;
	_nbrUsers = 0;
	_isPrivate = false;
	_isSecret = false;
	_isInviteOnly = false;
	_isModerate = false;
	_isTopicBlock = true;
	_isBlockOutside = true;
}

Channel::~Channel()
{
}

const std::string& Channel::getName() const
{
	return _name;
}

int Channel::setTopic(const std::string& topic, User* user)
{
	if (!isTopicBlock() || isAdmin(user))
	{
		_topic = topic;
		return 0;
	}
	return ERR_CHANOPRIVSNEEDED;
}

const std::string& Channel::getTopic() const
{
	return _topic;
}

int Channel::addUser(User* user, const std::string& password)
{
	if (_users.empty())
	{
		_users.insert(user);
		_admins.insert(user);
		if (!password.empty())
			_password = password;
		return 0;
	}
	if(_limit > 0 && _nbrUsers + 1 <= _limit)
		return ERR_CHANNELISFULL;
	if(isBaned(user))
		return ERR_BANNEDFROMCHAN;
	if (!_password.empty() && _password.compare(password))
		return ERR_BADCHANNELKEY;
	if (_isInviteOnly)
		return ERR_INVITEONLYCHAN;
	_nbrUsers++;
	_users.insert(user);
	user->addChannel(this);
	return 0;
	// return error para unirse
}

int Channel::removeUser(User* user)
{
	user->removeChannel(this);
	_users.erase(user);
	_admins.erase(user);
	_voiced.erase(user);
	_nbrUsers--;
	return 0;
}

void Channel::setVoiceUser(User* user)
{
	_voiced.insert(user);
}

void Channel::removeVoiceUser(User* user)
{
	_voiced.erase(user);
}

void Channel::setBanUser(User* user)
{
	_baned.insert(user);
}

void Channel::removeBanUser(User* user)
{
	_baned.erase(user);
}

int Channel::setMode(char mode, User* admin, User* user, const std::string& param)
{
	//TODO comprobar en que casos devuelve error por falta de privilegios
	switch (mode)
	{
	case 'p': //private
		if (isAdmin(admin))
		{
			_isPrivate = true;
			return 0;
		}
	break;
	case 'o': //operator
	//TODO comprobar que pasa si se hace admin a un usuario que no está
		if(user == NULL)
			return ERR_NEEDMOREPARAMS;
		if(user != NULL && isUser(user) && isAdmin(admin))
		{
			_admins.insert(user);
			return 0;
		}
	break;
	case 's': //secret
		if (isAdmin(admin))
		{
			_isSecret = true;
			return 0;
		}
	
	break;
	case 'i': //invite-only
		if (isAdmin(admin))
		{
			_isInviteOnly = true;
			return 0;
		}
	break;
	case 't': //topic settable by channel operator only flag;
		if (isAdmin(admin))
		{
			_isTopicBlock = true;
			return 0;
		}
	break;
	case 'n': //no messages to channel from clients on the outside
		if (isAdmin(admin))
		{
			_isBlockOutside = true;
			return 0;
		}
	break;
	case 'm': //moderated channel
		if (isAdmin(admin))
		{
			_isModerate = true;
			return 0;
		}
	break;
	case 'l': //set the user limit to channel
		if (param.empty())
			return ERR_NEEDMOREPARAMS;
		if (isAdmin(admin))
		{
			//TODO ver que pasa si no es un número
			_limit = atoi(param.c_str());
			return 0;
		}
	break;
	case 'b': //set a ban mask to keep users out
		if(user == NULL)
			return ERR_NEEDMOREPARAMS;
		if(isUser(user) && isAdmin(admin))
		{
			_baned.insert(user);
		}
	break;
	case 'v': //give/take the ability to speak on a moderated channel
		if(user == NULL)
			return ERR_NEEDMOREPARAMS;
		if(isUser(user) && isAdmin(admin))
		{
			_voiced.insert(user);
		}
	break;
	case 'k': //set a channel key (password)
		if (isAdmin(admin))
		{
			//TODO ver que pasa si no poner el parametro password
			_password = param;
			return 0;
		}
	break;
	default:
		break;
	}
	return 0;
}

int Channel::removeMode(char mode, User* admin, User* user)
{
	//TODO comprobar en que casos devuelve error por falta de privilegios
	switch (mode)
	{
	case 'p': //private
		if (isAdmin(admin))
		{
			_isPrivate = false;
			return 0;
		}
	break;
	case 'o': //operator
	//TODO comprobar que pasa si se hace admin a un usuario que no está
		if(user == NULL)
			return ERR_NEEDMOREPARAMS;
		if(isAdmin(admin) || admin == user)
		{
			_admins.erase(user);
			return 0;
		}
	break;
	case 's': //secret
		if (isAdmin(admin))
		{
			_isSecret = false;
			return 0;
		}
	
	break;
	case 'i': //invite-only
		if (isAdmin(admin))
		{
			_isInviteOnly = false;
			return 0;
		}
	break;
	case 't': //topic settable by channel operator only flag;
		if (isAdmin(admin))
		{
			_isTopicBlock = false;
			return 0;
		}
	break;
	case 'n': //no messages to channel from clients on the outside
		if (isAdmin(admin))
		{
			_isBlockOutside = false;
			return 0;
		}
	break;
	case 'm': //moderated channel
		if (isAdmin(admin))
		{
			_isModerate = false;
			return 0;
		}
	break;
	case 'l': //set the user limit to channel
		if (isAdmin(admin))
		{
			//TODO ver que pasa si no es un número
			_limit = -1;
			return 0;
		}
	break;
	case 'b': //set a ban mask to keep users out
		if(user == NULL)
			return ERR_NEEDMOREPARAMS;
		if(isUser(user) && isAdmin(admin))
		{
			_baned.insert(user);
		}
	break;
	case 'v': //give/take the ability to speak on a moderated channel
		if(user == NULL)
			return ERR_NEEDMOREPARAMS;
		if(isAdmin(admin) || admin == user)
		{
			_voiced.erase(user);
		}
	break;
	case 'k': //set a channel key (password)
		if (isAdmin(admin))
		{
			//TODO ver que pasa si no poner el parametro password
			_password = "";
			return 0;
		}
	break;
	default:
		break;
	}
	return 0;
}



bool Channel::isAdmin(User* admin) const
{
	return _admins.find(admin) != _admins.end();
}

bool Channel::isUser(User* user) const
{
	return _users.find(user) != _users.end();
}

bool Channel::isBaned(User* user) const
{
	return _baned.find(user) != _baned.end();
}

int Channel::addAdmin(User* user)
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


int Channel::setPrivate(bool mode, User* user)
{
	if (isAdmin(user))
	{
		_isPrivate = mode;
		return true;
	}
	return ERR_CHANOPRIVSNEEDED;
}

int Channel::setSecret(bool mode, User* user)
{
	if (isAdmin(user))
	{
		_isSecret = mode;
		return true;
	}
	return ERR_CHANOPRIVSNEEDED;
}

int Channel::setInviteOnly(bool mode, User* user)
{
	if (isAdmin(user))
	{
		_isInviteOnly = mode;
		return true;
	}
	return ERR_CHANOPRIVSNEEDED;
}

int Channel::setModerate(bool mode, User* user)
{
	if (isAdmin(user))
	{
		_isModerate = mode;
		return true;
	}
	return ERR_CHANOPRIVSNEEDED;
}

int Channel::setTopicBlock(bool mode, User* user)
{
	if (isAdmin(user))
	{
		_isTopicBlock = mode;
		return true;
	}
	return ERR_CHANOPRIVSNEEDED;
}

int Channel::setBlockOutside(bool mode, User* user)
{
	if (isAdmin(user))
	{
		_isBlockOutside = mode;
		return true;
	}
	return ERR_CHANOPRIVSNEEDED;
}

std::set<int> Channel::getUsers() const
{
	std::set<int> listUsers;
	std::set<User*>::iterator it;
	for(it = _users.begin(); it != _users.end(); it++)
	{
		User * user = *it;
		listUsers.insert(user->getFd());
	}
	return listUsers;
}

std::string Channel::getModes() const
{
	//:irc.example.com MODE #channel +nt
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
	//353     RPL_NAMREPLY
	//366     RPL_ENDOFNAMES
	//:irc.example.com 353 alice @ #channel :alice @dan
	std::string users;
	std::set<User*>::iterator it;
	for (it = _users.begin(); it != _users.end(); it++)
	{
		if (!users.empty())
			users += std::string(" ");
		User* user = *it;
		if (isAdmin(user))
			users += std::string("@");
		users += user->getNick();
	}
	return users;
}
