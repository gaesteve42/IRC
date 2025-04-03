/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCManager.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 12:03:23 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/03 15:17:24 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/IRCManager.hpp"

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
	std::cout << "🔗 Nouveau User associé au socket FD " << fd << std::endl;
}

void IRCManager::removeUser(int fd)
{
	if (users.find(fd) != users.end())
	{
		delete users[fd];
		users.erase(fd);
		std::cout << "❌ User supprimé (FD " << fd << ")" << std::endl;
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
		std::cout << "⛔️ FD " << fd << " : Authentification requise avant de pouvoir utiliser les commandes mon gourmand " << std::endl;
		return ;
	}
	if (channels.find(channelName) == channels.end())
		channels[channelName] = new Channel(channelName);

	if (channels[channelName]->addMember(user))
		std::cout << "📌 " << user->getNickname() << " a rejoint " << channelName << std::endl;
	else
		std::cout << "⚠️ " << user->getNickname() << " n'a pas pu rejoindre " << channelName << std::endl;
}

void IRCManager::partCommand(int fd, const std::string &channelName)
{
	User *user = getUser(fd);
	if (!user)
		return;
	if (!user->isAuthenticated())
	{
		std::cout << "⛔️ FD " << fd << " : Authentification requise avant de pouvoir utiliser les commandes mon gourmand " << std::endl;
		return ;
	}
	if (channels.find(channelName) != channels.end())
	{
		channels[channelName]->removeMember(user);
		std::cout << "📌 " << user->getNickname() << " a quitté " << channelName << std::endl;
	}
}

void IRCManager::privmsgCommand(int fd, const std::string &channelName, const std::string &message)
{
	User *sender = getUser(fd);
	if (!sender)
		return;
	if (!sender->isAuthenticated())
	{
		std::cout << "⛔️ FD " << fd << " : Authentification requise avant de pouvoir utiliser les commandes mon gourmand " << std::endl;
		return ;
	}
	if (channels.find(channelName) != channels.end())
	{
		const std::vector<User*> &members = channels[channelName]->getMembers();
		for (size_t i = 0; i < members.size(); i++)
		{
			User *receiver = members[i];
			if (receiver != sender)
			{
				// Remplacer par un vrai envoi via sockets (à intégrer côté serveur)
				std::cout << "💬 " << sender->getNickname() << " vers "
							<< receiver->getNickname() << " : " << message << std::endl;
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
		std::cout << "✅ Nickname défini à " << nickname << " pour FD " << fd << std::endl;
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
		std::cout << "✅ Username défini à " << username << " pour FD " << fd << std::endl;
		if (!user->getNickname().empty())
		user->setAuthenticated(true);
	}
}

void IRCManager::modeCommand(int fd, const std::string &channelName, const std::string &mode, const std::string &param)
{
	User *user = getUser(fd);
	if (!user || !user->isAuthenticated())
	{
		std::cout << "⛔️ Vous devez être authentifié !" << std::endl;
		return;
	}
	Channel *channel = channels[channelName];
	if (!channel)
	{
		std::cout << " Le Canal " << channelName << " n existe pas " << std::endl;
		return;
	}
	if (!channel->isMember(user) || !user->getIsOperator())
	{
		std::cout << "⛔️ Seuls les opérateurs peuvent changer les modes du canal !" << std::endl;
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
	std::cout << "✅ Mode " << mode << " appliqué sur " << channelName << " avec paramètre : " << param << std::endl;
}
