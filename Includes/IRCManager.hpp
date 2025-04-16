/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCManager.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yonieva <yonieva@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 12:04:06 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/16 16:28:00 by yonieva          ###   ########.fr       */
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
	User* getUserByNick(const std::string& nickname);
	Channel* getChannel(const std::string& name);

	void sendNamesReply(Channel* channel);
	void joinCommand(int fd, const std::string &channelName);
	void partCommand(int fd, const std::string &channelName, const std::string &reason = "");;
	void privmsgCommand(int fd, const std::string &channelName, const std::string &message);
	void nickCommand(int fd, const std::string &nickname);
	void userCommand(int fd, const std::string &username);
	void modeCommand(int fd, const std::string &channelName, const std::string &modeStr, const std::vector<std::string> &params);
	void kickCommand(int fd, const std::string &channelName, const std::string &targetNick, const std::string &reason);
	void inviteCommand(int fd, const std::string &channelName, const std::string &targetNick);
	void topicCommand(int fd, const std::string &channelName, const std::string &newTopic);
};

#endif
