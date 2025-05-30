/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:29:21 by gaesteve          #+#    #+#             */
/*   Updated: 2025/05/09 23:13:58 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Include.hpp"

Channel::Channel(const std::string &name) : channelName(name), topic(""), key(""), inviteOnly(false), topicRestricted(false), userLimit(0) {}

// ===== Getters =====
std::string Channel::getChannelName() const { return channelName; }
std::string Channel::getTopic() const { return topic; }
std::string Channel::getKey() const { return key; }
bool Channel::isInviteOnly() const { return inviteOnly; }
bool Channel::isTopicRestricted() const { return topicRestricted; }
size_t Channel::getUserLimit() const { return userLimit; }
const std::vector<User*> &Channel::getMembers() const { return members; }

// ===== Setters =====
void Channel::setTopic(const std::string &topic) { this->topic = topic; }
void Channel::setKey(const std::string &key) { this->key = key; }
void Channel::setInviteOnly(bool invite) { inviteOnly = invite; }
void Channel::setTopicRestricted(bool restricted) { topicRestricted = restricted; }
void Channel::setUserLimit(size_t limit) { userLimit = limit; }

// ===== Gestion des invitations =====
void Channel::addInvite(User* user) { invitedUsers.insert(user); }
bool Channel::isInvited(User* user) const { return invitedUsers.find(user) != invitedUsers.end(); }
void Channel::removeInvite(User* user) { invitedUsers.erase(user); }

// ===== Gestion des opérateurs =====
bool Channel::isOperator(User* user) const { return operators.count(user) > 0; }
void Channel::addOperator(User* user)
{
	if (isMember(user))
		operators.insert(user);
}
void Channel::removeOperator(User* user) { operators.erase(user); }

// ===== Utilitaire =====
bool Channel::addMember(User *user)
{
	if (userLimit != 0 && members.size() >= userLimit)
		return false;
	if (!isMember(user))
	{
		members.push_back(user);
		return true;
	}
	return false;
}

bool Channel::isMember(const User *user) const
{
	return std::find(members.begin(), members.end(), user) != members.end();
}

bool Channel::isInChannel(int fd) const
{
	for (size_t i = 0; i < members.size(); ++i)
	{
		if (members[i]->getFd() == fd)
			return true;
	}
	return false;
}

void Channel::removeMember(User *user)
{
	std::vector<User*>::iterator it = std::find(members.begin(), members.end(), user);
	if (it != members.end())
		members.erase(it);
}

// ===== Modes IRC actifs du canal =====
std::string Channel::getModeString() const
{
	std::string modes = "+";
	if (inviteOnly)
		modes += "i";
	if (topicRestricted)
		modes += "t";
	if (!key.empty())
		modes += "k";
	if (userLimit > 0)
		modes += "l";
	return modes;
}

std::string Channel::getModeParams() const
{
	std::string params;
	if (!key.empty())
		params += key + " ";
	if (userLimit > 0)
	{
		std::ostringstream oss;
		oss << userLimit;
		params += oss.str();
	}
	return params;
}
