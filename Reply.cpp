/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reply.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmateo-t <mmateo-t@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/15 16:51:52 by mmateo-t          #+#    #+#             */
/*   Updated: 2023/06/06 15:29:43 by mmateo-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Reply.hpp"

// Constructors
Reply::Reply() : _code(0)
{
}

Reply::Reply(const int code)
{
	this->_code = code;
}

Reply::Reply(const Reply &copy) : Message(copy)
{
	this->_targets = copy._targets;
	this->_code = copy._code;
}

// Destructor
Reply::~Reply()
{
}

// Operators
Reply &Reply::operator=(const Reply &assign)
{
	if (this != &assign)
	{
		this->_code = assign._code;
		this->_targets = assign._targets;
		this->_msg = assign._msg;
	}
	return *this;
}

// Getters and Setters
int Reply::getCode() const
{
	return this->_code;
}

void Reply::setCode(const int code)
{
	if (this->_code < 0)
	{
		logg(LOG_WARNING) << "Code cannot lower than zero.\n";
	}
	else
		this->_code = code;
}

void Reply::addTarget(const int &fd)
{
	this->_targets.insert(fd);
}

std::set<int> Reply::getTargets() const
{
	return this->_targets;
}

std::string Reply::getReplyMsg(std::string hostname) const
{
	if (this->_code > 0)
	{
		return ":" + hostname + " " + std::to_string(this->getCode()) + " " + this->getMsg() + "\n";
	}
	return "";
}

void Reply::setReplyMsg(const int code, const std::string message)
{
	this->_code = code;
	this->_msg = message;
}
