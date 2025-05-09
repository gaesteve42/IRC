/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCManager.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 12:03:23 by gaesteve          #+#    #+#             */
/*   Updated: 2025/05/09 23:17:39 by gaesteve         ###   ########.fr       */
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

// ==== Gestion des utilisateurs ====
void IRCManager::newUser(int fd)
{
	User *user = new User(fd);
	users[fd] = user;
	std::string msg = IRC_COLOR "03Bienvenue sur le serveur IRC_42 !" IRC_RESET "\r\n";
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

// ==== Gestion des canaux ====
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

void IRCManager::sendNamesReply(Channel* channel)
{
	const std::vector<User*>& members = channel->getMembers();
	for (size_t i = 0; i < members.size(); ++i)
	{
		std::string names;
		for (size_t j = 0; j < members.size(); ++j)
		{
			if (channel->isOperator(members[j]))
				names += "@";
			names += members[j]->getNickname();
			if (j + 1 < members.size())
				names += " ";
		}
		std::string nameReply = RPL_NAMREPLY(members[i]->getNickname(), "=", channel->getChannelName(), names);
		std::string endReply  = RPL_ENDOFNAMES(members[i]->getNickname(), channel->getChannelName());
		send(members[i]->getFd(), nameReply.c_str(), nameReply.length(), 0);
		send(members[i]->getFd(), endReply.c_str(), endReply.length(), 0);
	}
}

// ==== Commandes IRC principales ====
void IRCManager::joinCommand(int fd, const std::string &input)
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
	// Découper en channelName + userKey
	std::string channelName, userKey;
	size_t spacePos = input.find(' ');
	if (spacePos != std::string::npos)
	{
		channelName = input.substr(0, spacePos);
		userKey     = input.substr(spacePos + 1);
	}
	else
		channelName = input;
	if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&'))
	{
		std::string msg = ERR_BADCHANNAME(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	Channel *channel;
	// Créer le channel s’il n’existe pas
	if (channels.find(channelName) == channels.end())
	{
		channel = new Channel(channelName);
		channels[channelName] = channel;
		channel->addMember(user);
		channel->addOperator(user);
	}
	else
	{
		channel = channels[channelName];
		// Vérifier le mode +i
		if (channel->isInviteOnly() && !channel->isInvited(user))
		{
			std::string msg = ERR_INVITEONLYCHAN(channelName);
			send(fd, msg.c_str(), msg.length(), 0);
			return;
		}

		// Vérifier le mode +k
		if (!channel->getKey().empty() && channel->getKey() != userKey)
		{
			std::string msg = ERR_BADCHANNELKEY(channelName);
			send(fd, msg.c_str(), msg.length(), 0);
			return;
		}
		// Vérifier la limite +l
		if (channel->getUserLimit() > 0 && channel->getMembers().size() >= channel->getUserLimit())
		{
			std::string msg = ERR_CHANNELISFULL(channelName);
			send(fd, msg.c_str(), msg.length(), 0);
			return;
		}
		if (!channel->addMember(user))
		{
			std::string msg = ERR_CHANNELISFULL(channelName);
			send(fd, msg.c_str(), msg.length(), 0);
			return;
		}
	}
	// À partir d'ici l'utilisateur est membre confirmé du channel
	channel->removeInvite(user);
	// Envoyer un message JOIN visible par tous les membres du canal
	std::string joinMsg = ":" + user->getNickname() + "!" + user->getUsername()
						+ "@localhost JOIN :" + channelName + "\r\n";

	const std::vector<User*>& members = channel->getMembers();
	for (size_t i = 0; i < members.size(); ++i)
		send(members[i]->getFd(), joinMsg.c_str(), joinMsg.length(), 0);
	// Envoyer la liste des pseudos (RPL_NAMREPLY et RPL_ENDOFNAMES)
	std::string names;
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (channel->isOperator(members[i]))
			names += "@";
		names += members[i]->getNickname();
		if (i + 1 < members.size())
			names += " ";
	}
	std::string nameReply = RPL_NAMREPLY(user->getNickname(), "=", channelName, names);
	std::string endReply  = RPL_ENDOFNAMES(user->getNickname(), channelName);
	send(fd, nameReply.c_str(), nameReply.length(), 0);
	send(fd, endReply.c_str(), endReply.length(), 0);
	sendNamesReply(channel);
}

void IRCManager::partCommand(int fd, const std::string &channelName, const std::string &reason)
{
	User *user = getUser(fd);
	if (!user || !user->isAuthenticated())
		return;
	std::map<std::string, Channel*>::iterator it = channels.find(channelName);
	if (it == channels.end())
		return;
	Channel* channel = it->second;
	if (!channel->isMember(user))
		return;
	// construire le message PART
	std::string prefix = ":" + user->getNickname() + "!" + user->getUsername() + "@localhost";
	std::string partMsg = prefix + " PART " + channelName;
	if (!reason.empty())
		partMsg += " :" + reason; // On ajoute la raison si y en a une
	partMsg += "\r\n";
	// broadcast le message PART
	const std::vector<User*>& members = channel->getMembers();
	for (size_t i = 0; i < members.size(); ++i)
	{
		send(members[i]->getFd(), partMsg.c_str(), partMsg.length(), 0);
	}
	channel->removeMember(user);
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
		if (!chan->isInChannel(senderFd))
		{
			std::string err = ERR_CANNOTSENDTOCHAN(target);
			send(senderFd, err.c_str(), err.length(), 0);
			return;
		}
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
	if (!user)
		return;
	// Vérification si le nickname est déjà utilisé par un autre utilisateur
	for (std::map<int, User*>::iterator it = users.begin(); it != users.end(); ++it)
	{
		User *otherUser = it->second;
		if (otherUser->getNickname() == nickname && otherUser != user)
		{
			// Nickname déjà pris
			std::string err = ERR_NICKNAMEINUSE(nickname);
			send(fd, err.c_str(), err.length(), 0);
			return;
		}
	}
	user->setNickname(nickname);
	std::string msg = ":ircserv 001 " + nickname + " :Nickname défini\r\n";
	send(fd, msg.c_str(), msg.length(), 0);
	if (!user->getUsername().empty())
		user->setAuthenticated(true);
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

void IRCManager::modeCommand(int fd, const std::string &channelName, const std::string &modeStr, const std::vector<std::string> &params)
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
	if (!channel->isMember(user) || !channel->isOperator(user))
	{
		std::string msg = ERR_CHANOPRIVSNEEDED(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	bool add = true;
	size_t paramIndex = 0;
	for (size_t i = 0; i < modeStr.length(); ++i)
	{
		char m = modeStr[i];
		if (m == '+')
		{
			add = true;
			continue;
		}
		else if (m == '-')
		{
			add = false;
			continue;
		}

		switch (m)
		{
			case 'i':
				channel->setInviteOnly(add);
				break;
			case 't':
				channel->setTopicRestricted(add);
				break;
			case 'k':
				if (add)
				{
					if (paramIndex >= params.size())
					{
						std::string err = "ERROR :Missing key for +k\r\n";
						send(fd, err.c_str(), err.length(), 0);
						return;
					}
					channel->setKey(params[paramIndex++]);
				}
				else
				{
					channel->setKey("");
				}
				break;
			case 'l':
				if (add)
				{
					if (paramIndex >= params.size())
					{
						std::string err = "ERROR :Missing user limit for +l\r\n";
						send(fd, err.c_str(), err.length(), 0);
						return;
					}
					channel->setUserLimit(std::atoi(params[paramIndex++].c_str()));
				}
				else
				{
					channel->setUserLimit(0);
				}
				break;
				case 'o':
				{
					if (paramIndex >= params.size())
					{
						std::string err = "ERROR :Missing nickname for +o/-o\r\n";
						send(fd, err.c_str(), err.length(), 0);
						return;
					}
					const std::string &nick = params[paramIndex++];
					const std::vector<User*> &members = channel->getMembers();
					for (size_t j = 0; j < members.size(); ++j)
					{
						if (members[j]->getNickname() == nick)
						{
							if (add)
								channel->addOperator(members[j]);
							else
								channel->removeOperator(members[j]);
							if (add)
							{
								std::string notice = ":ircserv NOTICE #channel :" + nick + " est maintenant opérateur\r\n";
								for (size_t k = 0; k < members.size(); ++k)
									send(members[k]->getFd(), notice.c_str(), notice.size(), 0);
							}
							else
							{
								std::string notice = ":ircserv NOTICE #channel :" + nick + " a perdu ses droits d'op\r\n";
								for (size_t k = 0; k < members.size(); ++k)
									send(members[k]->getFd(), notice.c_str(), notice.size(), 0);
							}
							sendNamesReply(channel);

							break;
						}
					}
					break;
				}
			default:
			{
				std::string err = ERR_UNKNOWNMODE(std::string(1, m));
				send(fd, err.c_str(), err.length(), 0);
				return;
			}
		}
	}
	// Répondre à l'utilisateur
	std::string reply = ":ircserv 324 " + user->getNickname() + " " + channelName + " " + channel->getModeString() + "\r\n";
	send(fd, reply.c_str(), reply.length(), 0);
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
	if (!channel->isMember(sender) || !channel->isOperator(sender))
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
	if (!channel->isMember(sender) || !channel->isOperator(sender))
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
	if (channel->isMember(target))
	{
		std::string msg = ERR_USERONCHANNEL(targetNick, channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	channel->addInvite(target);
	//message a celui qui invite
	std::string msg = ":" + sender->getNickname() + " INVITE " + target->getNickname() + " :" + channelName + "\r\n";
	send(fd, msg.c_str(), msg.length(), 0);
	//message a celui qui est invite
	std::string inviteMsg = ":" + sender->getNickname() + " INVITE " + target->getNickname() + " :" + channelName + "\r\n";
	send(target->getFd(), inviteMsg.c_str(), inviteMsg.length(), 0);
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
		if (channel->isTopicRestricted() && !channel->isOperator(user))
		{
			std::string msg = ERR_CHANOPRIVSNEEDED(channelName);
			send(fd, msg.c_str(), msg.length(), 0);
			return;
		}
		channel->setTopic(newTopic);
		std::string msg = "" + user->getNickname() + " TOPIC " + channelName + " " + newTopic + "\r\n";
		send(fd, msg.c_str(), msg.length(), 0);
	}
}
