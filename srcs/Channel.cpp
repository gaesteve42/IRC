/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:29:21 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/03 14:59:23 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Channel.hpp"

Channel::Channel(const std::string &name) : channelName(name), topic(""), key(""), inviteOnly(false), topicRestricted(false), userLimit(0) {}

// Ajoute un membre au canal s'il y a de la place et qu'il n'est pas déjà membre
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

// Retire un membre du canal
void Channel::removeMember(User *user)
{
	std::vector<User*>::iterator it = std::find(members.begin(), members.end(), user);
	if (it != members.end())
		members.erase(it);
}

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

// ===== Utilitaire =====
// Vérifie si un user spécifique est déjà membre du canal
bool Channel::isMember(const User *user) const
{
	return std::find(members.begin(), members.end(), user) != members.end();
}

