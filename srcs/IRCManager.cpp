/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCManager.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yonieva <yonieva@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 12:03:23 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/15 19:48:06 by yonieva          ###   ########.fr       */
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

void IRCManager::joinCommand(int fd, const std::string &input)
{
	// Ex : input = "#channel" ou "#channel secretKey"
	User *user = getUser(fd);
	if (!user)
		return;
	if (!user->isAuthenticated())
	{
		std::string msg = ERR_NOTREGISTERED();
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	// 1) Découper en channelName + userKey
	std::string channelName, userKey;
	size_t spacePos = input.find(' ');
	if (spacePos != std::string::npos)
	{
		channelName = input.substr(0, spacePos);
		userKey     = input.substr(spacePos + 1);
	}
	else
	{
		channelName = input; // pas de key
	}
	// 2) Créer le channel s’il n’existe pas
	if (channels.find(channelName) == channels.end())
	{
		if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&'))
		{
			std::string msg = ERR_BADCHANNAME(channelName);
			send(fd, msg.c_str(), msg.length(), 0);
			return;
		}
		channels[channelName] = new Channel(channelName);
		// Le premier qui joint -> opérateur
		user->setOperator(true);
	}
	Channel *channel = channels[channelName];
	// 3) Vérifier le mode +i (inviteOnly)
	if (channel->isInviteOnly() && !channel->isInvited(user))
	{
		std::string msg = ERR_INVITEONLYCHAN(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	// 4) Vérifier le mode +k (clé)
	//    Si channel->getKey() n’est pas vide, on compare avec userKey
	if (!channel->getKey().empty())
	{
		if (userKey != channel->getKey())
		{
			// Mauvaise clé => impossible de joindre
			std::string msg = ERR_BADCHANNELKEY(channelName);
			send(fd, msg.c_str(), msg.length(), 0);
			return;
		}
	}
	// 5) Essayer d’ajouter l’utilisateur
	if (channel->addMember(user))
	{
		channel->removeInvite(user);
		// ------------------ Message JOIN visible par tous
		std::string joinMsg = ":" + user->getNickname() + "!" + user->getUsername()
								+ "@localhost JOIN :" + channelName + "\r\n";
		const std::vector<User*>& members = channel->getMembers();
		for (size_t i = 0; i < members.size(); ++i)
			send(members[i]->getFd(), joinMsg.c_str(), joinMsg.length(), 0);
		// ------------------ List des pseudos
		std::string names;
		for (size_t i = 0; i < members.size(); ++i)
		{
			names += members[i]->getNickname();
			if (i + 1 < members.size())
				names += " ";
		}
		std::string nameReply = RPL_NAMREPLY(user->getNickname(), "=", channelName, names);
		std::string endReply  = RPL_ENDOFNAMES(user->getNickname(), channelName);
		send(fd, nameReply.c_str(), nameReply.length(), 0);
		send(fd, endReply.c_str(), endReply.length(), 0);
	}
	else
	{
		std::string msg = ERR_CHANNELISFULL(channelName);
		send(fd, msg.c_str(), msg.length(), 0);
	}
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
		partMsg += " :" + reason;  // On ajoute le reason s’il existe
	partMsg += "\r\n";
	// broadcast le message PART
	const std::vector<User*>& members = channel->getMembers();
	for (size_t i = 0; i < members.size(); ++i)
	{
		send(members[i]->getFd(), partMsg.c_str(), partMsg.length(), 0);
	}
	// retire user
	channel->removeMember(user);
	// s’il est vide -> suppr
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

	// Vérification : le nickname est-il déjà utilisé par un autre utilisateur ?
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
	if (!channel->isMember(user) || !user->getIsOperator())
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
							members[j]->setOperator(add);
							// ICI on envoie un message de notification
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
	if (channel->isMember(target))
	{
		std::string msg = ERR_USERONCHANNEL(targetNick, channelName);
		send(fd, msg.c_str(), msg.length(), 0);
		return;
	}
	channel->addInvite(target);
	//mess a celui qui invite
	std::string msg = ":" + sender->getNickname() + " INVITE " + target->getNickname() + " :" + channelName + "\r\n";
	send(fd, msg.c_str(), msg.length(), 0);
	//mess a celui qui est invite
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
		if (channel->isTopicRestricted() && !user->getIsOperator())
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

void IRCManager::whoCommand(int fd, const std::string &channelName)
{
	User* user = getUser(fd);
	if (!user || !user->isAuthenticated())
		return;

	Channel* channel = NULL;
	if (channels.find(channelName) != channels.end())
		channel = channels[channelName];

	if (!channel)
	{
		std::string err = ERR_NOSUCHCHANNEL(channelName);
		send(fd, err.c_str(), err.length(), 0);
		return;
	}

	const std::vector<User*>& members = channel->getMembers();

	for (size_t i = 0; i < members.size(); ++i)
	{
		User* member = members[i];

		std::string flags = "H";
		if (member->getIsOperator())
			flags += "@";

		std::string reply = ":ircserv 352 " + user->getNickname() + " " + channelName + " ";
		reply += member->getUsername() + " localhost ircserv " + member->getNickname() + " ";
		reply += flags + " :0 " + member->getNickname() + "\r\n";

		send(fd, reply.c_str(), reply.length(), 0);
	}

	std::string endReply = ":ircserv 315 " + user->getNickname() + " " + channelName + " :End of WHO list\r\n";
	send(fd, endReply.c_str(), endReply.length(), 0);

	// → on envoie aussi RPL_NAMREPLY ici !
	std::string names;
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (members[i]->getIsOperator())
			names += "@";
		names += members[i]->getNickname();
		if (i + 1 < members.size())
			names += " ";
	}
	std::string nameRe = RPL_NAMREPLY(user->getNickname(), "=", channelName, names);
	std::string endRe = RPL_ENDOFNAMES(user->getNickname(), channelName);

	send(fd, nameRe.c_str(), nameRe.length(), 0);
	send(fd, endRe.c_str(), endRe.length(), 0);

}




