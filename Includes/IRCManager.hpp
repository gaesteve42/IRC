/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCManager.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 12:04:06 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/03 16:59:39 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCMANAGER_HPP
#define IRCMANAGER_HPP

#include "../Includes/Include.hpp"

class IRCManager
{
private:
	std::map<int, User*> users;
	std::map<std::string, Channel*> channels;

public:
	IRCManager();
	~IRCManager();

	std::map<std::string, Channel*>& getChannels();
	void newUser(int fd);
	void removeUser(int fd);
	User* getUser(int fd);

	void joinCommand(int fd, const std::string &channelName);
	void partCommand(int fd, const std::string &channelName);
	void privmsgCommand(int fd, const std::string &channelName, const std::string &message);
	void nickCommand(int fd, const std::string &nickname);
	void userCommand(int fd, const std::string &username);
	void modeCommand(int fd, const std::string &channelName, const std::string &mode, const std::string &param = "");
	void kickCommand(int fd, const std::string &channelName, const std::string &targetNick, const std::string &reason);
	void inviteCommand(int fd, const std::string &channelName, const std::string &targetNick);
	void topicCommand(int fd, const std::string &channelName, const std::string &newTopic);

};

#endif
