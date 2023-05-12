/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListUsers.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmateo-t <mmateo-t@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 12:42:51 by mmateo-t          #+#    #+#             */
/*   Updated: 2023/05/12 20:15:16 by mmateo-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIST_USERS_HPP
#define LIST_USERS_HPP
#include "lib2.h"
#define maxFd

class User;

class ListUsers
{
private:
	std::map<std::string, User *> _usersByName;
	std::map<size_t, User *> _usersByFd;
	bool addUser(User *user);

public:
	typedef std::map<std::string, User *>::iterator iterator;
	ListUsers(/* args */);
	~ListUsers();
	User *createUser(const size_t fd);
	int setNick(const std::string &nick, const size_t fd);
	int setUser(const std::string &name, const size_t fd);
	bool hasUser() const;
	User *getUser(const std::string &nick);
	User *getUser(const size_t fd);
	int removeUser(const size_t fd);
	iterator begin();
	iterator end();
};

#endif
