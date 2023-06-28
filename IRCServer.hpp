/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 12:43:11 by mmateo-t          #+#    #+#             */
/*   Updated: 2023/06/28 16:43:51 by mvillaes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP
#define MAX_USERS 8
#define MAXMSGSIZE 512
#define HOST_SIZE 128
#define DEFAULT_PORT 6667

#include "Socket.hpp"
#include "include/Logger.hpp"
#include "include/Colors.hpp"
#include "Message.hpp"
#include "CommandHandler.hpp"
#include <poll.h>

class CommandHandler;
class ListUsers;
class ListChannels;
class Message;

class IRCServer
{
private:
	CommandHandler *_cmdHandler;
	ListUsers *_listUsers;
	ListChannels *_listChannels;

	Socket *_serverSocket;

	// server data
	struct hostent *host;
	struct in_addr _addr;
	char _hostname[HOST_SIZE];

	// poll fds
	struct pollfd _pollFds[MAX_USERS];
	int _nfds;

	// receive buffer
	char _buf[MAXMSGSIZE];

	//Login
	std::string _password;

	bool startServer();
	void pollLoop();
	void recvMessage(std::string s, int fd);
	void throwError(std::string msg);
	void setUpPoll();
	void acceptConnection();
	void setNonBlocking(int fdIn);
	void processMessage(std::string buff, int fd);
	std::string getHostname() const;
	void disconnect(const int fd);
	bool checkLogin(const std::string cmd, const int fd);

public:
	IRCServer(const uint16_t port, const std::string password);
	~IRCServer();
	
	friend class CommandHandler;
};

#endif
