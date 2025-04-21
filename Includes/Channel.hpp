/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:29:15 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/21 13:33:01 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "../Includes/Include.hpp"

class Channel
{
private:
	std::string channelName; // Nom du canal (#general, #games, etc.)
	std::string topic; // Sujet/thème du canal affiché aux utilisateurs
	std::string key; // Mot de passe pour rejoindre le canal
	bool inviteOnly; // Accès uniquement sur invitation (mode +i)
	bool topicRestricted; // Seuls les opérateurs peuvent changer le sujet (mode +t)
	size_t userLimit; // Nombre max de membres autorisés (mode +l, 0 = illimité)
	std::vector<User*> members; // Liste des membres présents dans le canal
	std::set<User*> invitedUsers;
	std::set<User*> operators;
public:
	// Constructeur
	Channel(const std::string &name);
	// Gestion des membres
	bool addMember(User *user); // Ajoute un membre si possible
	void removeMember(User *user); // Retire un membre
	bool isMember(const User *User) const; // Vérifie si un User est déjà membre du canal
	bool isInChannel(int fd) const;
	// Gestion des invitations
	void addInvite(User* user); // Ajoute un User à la liste des invités
	bool isInvited(User* user) const; // Vérifie si un User est invité
	void removeInvite(User* user); // Retire un User de la liste d'invités
	//Gestion des opérateurs
	bool isOperator(User* user) const;
	void addOperator(User* user);
	void removeOperator(User* user);
	// Getters
	std::string getChannelName() const;
	std::string getTopic() const;
	std::string getKey() const;
	bool isInviteOnly() const;
	bool isTopicRestricted() const;
	size_t getUserLimit() const;
	const std::vector<User*> &getMembers() const;
	//modeCommand
	std::string getModeString() const;
	std::string getModeParams() const;
	// Setters
	void setTopic(const std::string &topic);
	void setKey(const std::string &key);
	void setInviteOnly(bool invite);
	void setTopicRestricted(bool restricted);
	void setUserLimit(size_t limit);
};

#endif

