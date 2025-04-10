/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCManager.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 12:03:23 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/10 18:46:23 by gaesteve         ###   ########.fr       */
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
	User *user = new User(fd);
	users[fd] = user;
	std::string msg = ":ircserv NOTICE * :Bienvenue sur le serveur IRC_42 !\r\n";
	send(fd, msg.c_str(), msg.length(), 0);
}

void IRCManager::removeUser(int fd)
{
	if (users.find(fd) != users.end())
	{
		delete users[fd];
		users.erase(fd);
		std::string msg = ":ircserv: Utilisateur déconnecté\r\n";
		send(fd, msg.c_str(), msg.length(), 0);
	}
}

User* IRCManager::getUser(int fd)
{
	if (users.find(fd) != users.end())
		return users[fd];
	return NULL;
}

User* IRCManager::getUserByNick(const std::string& nickname)
{
	for (std::map<int, User*>::iterator it = users.begin(); it != users.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return it->second;
	}
	return NULL;
}

Channel* IRCManager::getChannel(const std::string& name)
{
	std::map<std::string, Channel*>::iterator it = channels.find(name);
	if (it != channels.end())
		return it->second;
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
	{
		channels[channelName] = new Channel(channelName);
		user->setOperator(true);
	}
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
		// Message JOIN visible par tous les membres
		std::string joinMsg = ":" + user->getNickname() + "!" + user->getUsername() + "@localhost JOIN :" + channelName + "\r\n";
		// Envoyer à tous les membres (y compris celui qui rejoint)
		const std::vector<User*>& members = channel->getMembers();
		for (size_t i = 0; i < members.size(); ++i)
		send(members[i]->getFd(), joinMsg.c_str(), joinMsg.length(), 0);
		// Liste des pseudos du channel
		std::string names;
		for (size_t i = 0; i < members.size(); ++i)
		{
			names += members[i]->getNickname();
			if (i + 1 < members.size())
				names += " ";
		}
		// Réponse standard pour HexChat
		std::string nameReply = RPL_NAMREPLY(user->getNickname(), "=", channelName, names);
		std::string endReply = RPL_ENDOFNAMES(user->getNickname(), channelName);
		send(fd, nameReply.c_str(), nameReply.length(), 0);
		send(fd, endReply.c_str(), endReply.length(), 0);
	}
	else
	{
		std::string msg = ERR_CHANNELISFULL(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
	}
}

void IRCManager::partCommand(int fd, const std::string &channelName)
{
	User* user = getUser(fd);
	if (!user || !user->isAuthenticated())
	{
		std::string msg = ERR_NOTREGISTERED();
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	std::map<std::string, Channel*>::iterator it = channels.find(channelName);
	if (it == channels.end())
	{
		std::string msg = ERR_NOSUCHCHANNEL(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	Channel* channel = it->second;
	if (!channel->isMember(user))
	{
		std::string msg = ERR_NOTONCHANNEL(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	// Envoyer le message PART à tous les membres du canal
	std::string prefix = ":" + user->getNickname() + "!" + user->getUsername() + "@localhost";
	std::string partMsg = prefix + " PART " + channelName + "\r\n";

	const std::vector<User*>& members = channel->getMembers();
	for (size_t i = 0; i < members.size(); ++i)
	{
		send(members[i]->getFd(), partMsg.c_str(), partMsg.length(), 0);
	}
	// Retirer le membre du canal
	channel->removeMember(user);
	// Si le canal est vide → suppression
	if (channel->getMembers().empty())
	{
		delete channel;
		channels.erase(it);
	}
}

void IRCManager::privmsgCommand(int senderFd, const std::string& target, const std::string& message)
{
	User* sender = getUser(senderFd);
	if (!sender || !sender->isAuthenticated())
		return;
	std::string prefix = ":" + sender->getNickname() + "!" + sender->getUsername() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";
	// Cas 1 : C’est un channel
	if (!target.empty() && (target[0] == '#' || target[0] == '&'))
	{
		Channel* chan = getChannel(target);
		if (!chan)
		{
			std::string err = ERR_NOSUCHCHANNEL(target);
			send(senderFd, err.c_str(), err.length(), 0);
			return;
		}
		// Le client doit être dans le canal
		if (!chan->isInChannel(senderFd))
		{
			std::string err = ERR_CANNOTSENDTOCHAN(target);
			send(senderFd, err.c_str(), err.length(), 0);
			return;
		}
		// Envoie à tous les membres sauf l’émetteur
		const std::vector<User*>& members = chan->getMembers();
		for (size_t i = 0; i < members.size(); ++i)
		{
			if (members[i]->getFd() != senderFd)
				send(members[i]->getFd(), prefix.c_str(), prefix.length(), 0);
		}
	}
	// Cas 2 : C’est un utilisateur
	else
	{
		User* recipient = getUserByNick(target);
		if (!recipient)
		{
			std::string err = ERR_NOSUCHNICK(target);
			send(senderFd, err.c_str(), err.length(), 0);
			return;
		}
		send(recipient->getFd(), prefix.c_str(), prefix.length(), 0);
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
		std::string msg = ":ircserv NOTICE " + user->getUsername() + " :Username défini\r\n";
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
	std::string prefix = ":" + sender->getNickname() + "!" + sender->getUsername() + "@localhost";
	std::string msg = prefix + " KICK " + channelName + " " + target->getNickname() + " :" + reason + "\r\n";
	for (size_t i = 0; i < members.size(); ++i)
	{
		send(members[i]->getFd(), msg.c_str(), msg.length(), 0);
	}
	channel->removeMember(target);
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
