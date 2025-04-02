/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:29:21 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/02 11:58:40 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Channel.hpp"

Channel::Channel(const std::string &name)
	: channelName(name), topic(""), key(""), inviteOnly(false),
	  topicRestricted(false), userLimit(0) {}

// Ajoute un membre au canal s'il y a de la place et qu'il n'est pas déjà membre
bool Channel::addMember(Client *client)
{
	if (userLimit != 0 && members.size() >= userLimit)
		return false;
	if (!isMember(client))
	{
		members.push_back(client);
		return true;
	}
	return false;
}

// Retire un membre du canal
void Channel::removeMember(Client *client)
{
	std::vector<Client*>::iterator it = std::find(members.begin(), members.end(), client);
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
const std::vector<Client*> &Channel::getMembers() const { return members; }

// ===== Setters =====
void Channel::setTopic(const std::string &topic) { this->topic = topic; }
void Channel::setKey(const std::string &key) { this->key = key; }
void Channel::setInviteOnly(bool invite) { inviteOnly = invite; }
void Channel::setTopicRestricted(bool restricted) { topicRestricted = restricted; }
void Channel::setUserLimit(size_t limit) { userLimit = limit; }

// ===== Utilitaire =====
// Vérifie si un client spécifique est déjà membre du canal
bool Channel::isMember(const Client *client) const
{
	return std::find(members.begin(), members.end(), client) != members.end();
}

