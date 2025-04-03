/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCManager.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 12:03:23 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/03 16:36:33 by gaesteve         ###   ########.fr       */
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
	std::cout << "🔗 Nouveau User associé au socket FD " << fd << std::endl; // [TO REPLACE: send welcome message]
}

void IRCManager::removeUser(int fd)
{
	if (users.find(fd) != users.end())
	{
		delete users[fd];
		users.erase(fd);
		std::cout << "❌ User supprimé (FD " << fd << ")" << std::endl; // [TO REPLACE: send quit message]
	}
}

User* IRCManager::getUser(int fd)
{
	if (users.find(fd) != users.end())
		return users[fd];
	return NULL;
}

void IRCManager::joinCommand(int fd, const std::string &channelName)
{
	User *user = getUser(fd);
	if (!user)
		return;
	if (!user->isAuthenticated())
	{
		std::string msg = ERR_NOTREGISTERED();
		std::cout << msg; // [TO REPLACE: send error message]
		return;
	}
	if (channels.find(channelName) == channels.end())
		channels[channelName] = new Channel(channelName);
	Channel *channel = channels[channelName];
	if (channel->isInviteOnly() && !channel->isInvited(user))
	{
		std::string msg = ERR_INVITEONLYCHAN(channelName);
		std::cout << msg; // [TO REPLACE: send invite-only error]
		return;
	}
	if (channel->addMember(user))
	{
		channel->removeInvite(user); // auto suppression de l'invitation
		std::cout << "📌 " << user->getNickname() << " a rejoint " << channelName << std::endl; // [TO REPLACE: send join message]
	}
	else
	{
		std::cout << "⚠️ " << user->getNickname() << " n'a pas pu rejoindre " << channelName << std::endl; // [TO REPLACE: send error message]
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
		std::cout << msg; // [TO REPLACE: send error message]
		return;
	}
	if (channels.find(channelName) != channels.end())
	{
		channels[channelName]->removeMember(user);
		std::cout << "📌 " << user->getNickname() << " a quitté " << channelName << std::endl; // [TO REPLACE: send part message]
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
		std::cout << msg; // [TO REPLACE: send error message]
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
				std::cout << "💬 " << sender->getNickname() << " vers "
						  << receiver->getNickname() << " : " << message << std::endl; // [TO REPLACE: send privmsg to receiver]
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
		std::cout << "✅ Nickname défini à " << nickname << " pour FD " << fd << std::endl; // [TO REPLACE: send nick confirmation]
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
		std::cout << "✅ Username défini à " << username << " pour FD " << fd << std::endl; // [TO REPLACE: send user confirmation]
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
		std::cout << msg; // [TO REPLACE: send error message]
		return;
	}
	Channel *channel = channels[channelName];
	if (!channel)
	{
		std::string msg = ERR_NOSUCHCHANNEL(channelName);
		std::cout << msg; // [TO REPLACE: send error message]
		return;
	}
	if (!channel->isMember(user) || !user->getIsOperator())
	{
		std::string msg = ERR_CHANOPRIVSNEEDED(channelName);
		std::cout << msg; // [TO REPLACE: send error message]
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
	std::cout << "✅ Mode " << mode << " appliqué sur " << channelName << " avec paramètre : " << param << std::endl; // [TO REPLACE: send mode confirmation]
}

void IRCManager::kickCommand(int fd, const std::string &channelName, const std::string &targetNick, const std::string &reason)
{
	User *sender = getUser(fd);
	if (!sender || !sender->isAuthenticated())
	{
		std::string msg = ERR_NOTREGISTERED();
		std::cout << msg; // [TO REPLACE: send error message]
		return;
	}
	if (channels.find(channelName) == channels.end())
	{
		std::string msg = ERR_NOSUCHCHANNEL(channelName);
		std::cout << msg; // [TO REPLACE: send error message]
		return;
	}
	Channel *channel = channels[channelName];
	if (!channel->isMember(sender) || !sender->getIsOperator())
	{
		std::string msg = ERR_CHANOPRIVSNEEDED(channelName);
		std::cout << msg; // [TO REPLACE: send error message]
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
		std::cout << msg; // [TO REPLACE: send error message]
		return;
	}
	channel->removeMember(target);
	std::cout << "👢 " << sender->getNickname()
			  << " a KICK " << target->getNickname()
			  << " de " << channelName
			  << " pour : " << reason << std::endl; // [TO REPLACE: send kick notice]
}

void IRCManager::inviteCommand(int fd, const std::string &channelName, const std::string &targetNick)
{
	User *sender = getUser(fd);
	if (!sender || !sender->isAuthenticated())
	{
		std::string msg = ERR_NOTREGISTERED();
		std::cout << msg; // [TO REPLACE: send error message]
		return;
	}
	if (channels.find(channelName) == channels.end())
	{
		std::string msg = ERR_NOSUCHCHANNEL(channelName);
		std::cout << msg; // [TO REPLACE: send error message]
		return;
	}
	Channel *channel = channels[channelName];
	if (!channel->isMember(sender) || !sender->getIsOperator())
	{
		std::string msg = ERR_CHANOPRIVSNEEDED(channelName);
		std::cout << msg; // [TO REPLACE: send error message]
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
		std::cout << msg; // [TO REPLACE: send error message]
		return;
	}
	channel->addInvite(target);
	std::cout << "✉️  " << sender->getNickname() << " a invité " << targetNick << " dans " << channelName << std::endl; // [TO REPLACE: send invite]
}

void IRCManager::topicCommand(int fd, const std::string &channelName, const std::string &newTopic)
{
	User *user = getUser(fd);
	if (!user || !user->isAuthenticated())
	{
		std::string msg = ERR_NOTREGISTERED();
		std::cout << msg; // [TO REPLACE: send error message]
		return;
	}
	if (channels.find(channelName) == channels.end())
	{
		std::string msg = ERR_NOSUCHCHANNEL(channelName);
		std::cout << msg; // [TO REPLACE: send error message]
		return;
	}
	Channel *channel = channels[channelName];
	if (!channel->isMember(user))
	{
		std::string msg = ERR_NOTONCHANNEL(channelName);
		std::cout << msg; // [TO REPLACE: send error message]
		return;
	}
	if (newTopic.empty())
	{
		if (channel->getTopic().empty())
		{
			std::string msg = RPL_NOTOPIC(channelName);
			std::cout << msg; // [TO REPLACE: send no topic info]
		}
		else
		{
			std::string msg = RPL_TOPIC(channelName, channel->getTopic());
			std::cout << msg; // [TO REPLACE: send current topic]
		}
	}
	else
	{
		if (channel->isTopicRestricted() && !user->getIsOperator())
		{
			std::string msg = ERR_CHANOPRIVSNEEDED(channelName);
			std::cout << msg; // [TO REPLACE: send error message]
			return;
		}
		channel->setTopic(newTopic);
		std::cout << "📝 Sujet de " << channelName << " changé en : " << newTopic << std::endl; // [TO REPLACE: send topic change]
	}
}
