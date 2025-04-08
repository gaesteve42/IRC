/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCManager.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 12:03:23 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/08 17:58:02 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Include.hpp"

IRCManager::IRCManager() {}

IRCManager::~IRCManager()
{
	for (std::map<int, User*>::iterator it = users.begin(); it != users.end(); ++it)
		delete it->second;
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
		delete it->second;
}

void IRCManager::newUser(int fd)
{
	User *user = new User();
	users[fd] = user;
	std::string msg = ":ircserv NOTICE * :Bienvenue sur le serveur IRC !\r\n";
	send(fd, msg.c_str(), msg.length(), 0);
}

void IRCManager::removeUser(int fd)
{
	if (users.find(fd) != users.end())
	{
		delete users[fd];
		users.erase(fd);
		std::string msg = ":ircserv NOTICE * :Utilisateur déconnecté\r\n";
		send(fd, msg.c_str(), msg.length(), 0);
	}
}

User* IRCManager::getUser(int fd)
{
	if (users.find(fd) != users.end())
		return users[fd];
	return NULL;
}

std::map<std::string, Channel*> &IRCManager::getChannels()
{
	return channels;
}

void IRCManager::joinCommand(int fd, const std::string &channelName)
{
	User *user = getUser(fd);
	if (!user)
		return;
	if (!user->isAuthenticated())
	{
		std::string msg = ERR_NOTREGISTERED();
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	if (channels.find(channelName) == channels.end())
		channels[channelName] = new Channel(channelName);
	Channel *channel = channels[channelName];
	if (channel->isInviteOnly() && !channel->isInvited(user))
	{
		std::string msg = ERR_INVITEONLYCHAN(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	if (channel->addMember(user))
	{
		channel->removeInvite(user); // supprime l invit direct
		std::string msg = ":ircserv NOTICE " + user->getNickname() + " :Vous avez rejoint " + channelName + "\r\n";
		send(fd, msg.c_str(), msg.length(), 0);
	}
	else
	{
		std::string msg = ERR_CHANNELISFULL(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
	}
}

void IRCManager::partCommand(int fd, const std::string &channelName)
{
	User *user = getUser(fd);
	if (!user)
		return;
	if (!user->isAuthenticated())
	{
		std::string msg = ERR_NOTREGISTERED();
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	if (channels.find(channelName) != channels.end())
	{
		channels[channelName]->removeMember(user);
		std::string msg = ":ircserv NOTICE " + user->getNickname() + " :Vous avez quitté " + channelName + "\r\n";
		send(fd, msg.c_str(), msg.length(), 0);
	}
}

void IRCManager::privmsgCommand(int fd, const std::string &channelName, const std::string &message)
{
	User *sender = getUser(fd);
	if (!sender)
		return;
	if (!sender->isAuthenticated())
	{
		std::string msg = ERR_NOTREGISTERED();
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	if (channels.find(channelName) != channels.end())
	{
		const std::vector<User*> &members = channels[channelName]->getMembers();
		for (size_t i = 0; i < members.size(); i++)
		{
			User *receiver = members[i];
			if (receiver != sender)
			{
				std::string msg = ":" + sender->getNickname() + " PRIVMSG " + channelName + " :" + message + "\r\n";
				send(fd, msg.c_str(), msg.length(), 0);
			}
		}
	}
}

void IRCManager::nickCommand(int fd, const std::string &nickname)
{
	User *user = getUser(fd);
	if (user)
	{
		user->setNickname(nickname);
		std::string msg = ":ircserv 001 " + nickname + " :Nickname défini\r\n";
		send(fd, msg.c_str(), msg.length(), 0);
		if (!user->getUsername().empty())
			user->setAuthenticated(true);
	}
}

void IRCManager::userCommand(int fd, const std::string &username)
{
	User *user = getUser(fd);
	if (user)
	{
		user->setUsername(username);
		std::string msg = ":ircserv NOTICE " + user->getNickname() + " :Username défini\r\n";
		send(fd, msg.c_str(), msg.length(), 0);
		if (!user->getNickname().empty())
			user->setAuthenticated(true);
	}
}

void IRCManager::modeCommand(int fd, const std::string &channelName, const std::string &mode, const std::string &param)
{
	User *user = getUser(fd);
	if (!user || !user->isAuthenticated())
	{
		std::string msg = ERR_NOTREGISTERED();
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	Channel *channel = channels[channelName];
	if (!channel)
	{
		std::string msg = ERR_NOSUCHCHANNEL(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	if (!channel->isMember(user) || !user->getIsOperator())
	{
		std::string msg = ERR_CHANOPRIVSNEEDED(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	if (mode == "+i")
		channel->setInviteOnly(true);
	else if (mode == "-i")
		channel->setInviteOnly(false);
	else if (mode == "+t")
		channel->setTopicRestricted(true);
	else if (mode == "-t")
		channel->setTopicRestricted(false);
	else if (mode == "+k")
		channel->setKey(param);
	else if (mode == "-k")
		channel->setKey("");
	else if (mode == "+l")
		channel->setUserLimit(std::atoi(param.c_str()));
	else if (mode == "-l")
		channel->setUserLimit(0);
	else if (mode == "+o")
	{
		for (size_t i = 0; i < channel->getMembers().size(); i++)
		{
			User *member = channel->getMembers()[i];
			if (member->getNickname() == param)
				member->setOperator(true);
		}
	}
	else if (mode == "-o")
	{
		for (size_t i = 0; i < channel->getMembers().size(); i++)
		{
			User *member = channel->getMembers()[i];
			if (member->getNickname() == param)
				member->setOperator(false);
		}
	}
	std::string msg = ":ircserv 324 " + channelName + " " + mode + (param.empty() ? "" : " " + param) + "\r\n";
	send(fd, msg.c_str(), msg.length(), 0);
}

void IRCManager::kickCommand(int fd, const std::string &channelName, const std::string &targetNick, const std::string &reason)
{
	User *sender = getUser(fd);
	if (!sender || !sender->isAuthenticated())
	{
		std::string msg = ERR_NOTREGISTERED();
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	if (channels.find(channelName) == channels.end())
	{
		std::string msg = ERR_NOSUCHCHANNEL(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	Channel *channel = channels[channelName];
	if (!channel->isMember(sender) || !sender->getIsOperator())
	{
		std::string msg = ERR_CHANOPRIVSNEEDED(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	User *target = NULL;
	const std::vector<User*> &members = channel->getMembers();
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (members[i]->getNickname() == targetNick)
		{
			target = members[i];
			break;
		}
	}
	if (!target)
	{
		std::string msg = ERR_USERNOTINCHANNEL(targetNick, channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	channel->removeMember(target);
	std::string msg = ":" + sender->getNickname() + " KICK " + channelName + " " + target->getNickname() + " :" + reason + "\r\n";
	send(fd, msg.c_str(), msg.length(), 0);
}

void IRCManager::inviteCommand(int fd, const std::string &channelName, const std::string &targetNick)
{
	User *sender = getUser(fd);
	if (!sender || !sender->isAuthenticated())
	{
		std::string msg = ERR_NOTREGISTERED();
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	if (channels.find(channelName) == channels.end())
	{
		std::string msg = ERR_NOSUCHCHANNEL(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	Channel *channel = channels[channelName];
	if (!channel->isMember(sender) || !sender->getIsOperator())
	{
		std::string msg = ERR_CHANOPRIVSNEEDED(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	User *target = NULL;
	for (std::map<int, User*>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (it->second->getNickname() == targetNick)
		{
			target = it->second;
			break;
		}
	}
	if (!target)
	{
		std::string msg = ERR_NOSUCHNICK(targetNick);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	channel->addInvite(target);
	std::string msg = ":" + sender->getNickname() + " INVITE " + target->getNickname() + " :" + channelName + "\r\n";
	send(fd, msg.c_str(), msg.length(), 0);
}

void IRCManager::topicCommand(int fd, const std::string &channelName, const std::string &newTopic)
{
	User *user = getUser(fd);
	if (!user || !user->isAuthenticated())
	{
		std::string msg = ERR_NOTREGISTERED();
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	if (channels.find(channelName) == channels.end())
	{
		std::string msg = ERR_NOSUCHCHANNEL(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	Channel *channel = channels[channelName];
	if (!channel->isMember(user))
	{
		std::string msg = ERR_NOTONCHANNEL(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	if (newTopic.empty())
	{
		if (channel->getTopic().empty())
		{
			std::string msg = RPL_NOTOPIC(channelName);
			send(fd, msg.c_str(), msg.length(), 0);
		}
		else
		{
			std::string msg = RPL_TOPIC(channelName, channel->getTopic());
			send(fd, msg.c_str(), msg.length(), 0);
		}
	}
	else
	{
		if (channel->isTopicRestricted() && !user->getIsOperator())
		{
			std::string msg = ERR_CHANOPRIVSNEEDED(channelName);
			send(fd, msg.c_str(), msg.length(), 0);
			return;
		}
		channel->setTopic(newTopic);
		std::string msg = ":" + user->getNickname() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
		send(fd, msg.c_str(), msg.length(), 0);
	}
}
