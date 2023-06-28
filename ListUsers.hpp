/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListUsers.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 12:42:51 by mmateo-t          #+#    #+#             */
/*   Updated: 2023/06/28 13:29:43 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIST_USERS_HPP
#define LIST_USERS_HPP
#include "lib2.h"
#define MAXUSERS 512

class User;

class ListUsers
{
private:
	std::map<std::string, User *> _usersByNick;
	std::map<size_t, User *> _usersByFd;
	std::set<User *> _listOfUsers;
	bool addUser(User *user);
	int _userCounter;

public:
	typedef std::map<std::string, User *>::iterator iterator;
	ListUsers();
	~ListUsers();
	bool createUser(const size_t fd);
	int setNick(const std::string &nick, const size_t fd);
	int setUser(const std::string &name, const size_t fd);
	User *getUser(const std::string &nick);
	User *getUser(const size_t fd);
	int removeUser(const size_t fd);
};

#endif
