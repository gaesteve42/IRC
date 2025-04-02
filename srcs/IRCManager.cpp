/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCManager.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 12:03:23 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/02 14:17:16 by gaesteve         ###   ########.fr       */
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

// Gestion directe des commandes IRC parsées par ton mate

void IRCManager::joinCommand(int fd, const std::string &channelName)
{
	User *user = getUser(fd);
	if (!user)
		return;

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

