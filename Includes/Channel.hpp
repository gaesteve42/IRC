/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:29:15 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/02 11:59:30 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "User.hpp"
#include <vector>
#include <algorithm>

// Classe représentant un canal IRC (c'est un salon textuel sur Discord EN GROS)
class Channel
{
private:
	std::string channelName;       // Nom du canal (#general, #games, etc.)
	std::string topic;             // Sujet/thème du canal affiché aux utilisateurs
	std::string key;               // Mot de passe pour rejoindre le canal
	bool inviteOnly;               // Accès uniquement sur invitation (mode +i)
	bool topicRestricted;          // Seuls les opérateurs peuvent changer le sujet (mode +t)
	size_t userLimit;              // Nombre max de membres autorisés (mode +l, 0 = illimité)
	std::vector<Client*> members;  // Liste des membres présents dans le canal

public:
	// Constructeur
	Channel(const std::string &name);

	// Gestion des membres
	bool addMember(Client *client);       // Ajoute un membre si possible
	void removeMember(Client *client);    // Retire un membre

	// Getters
	std::string getChannelName() const;
	std::string getTopic() const;
	std::string getKey() const;
	bool isInviteOnly() const;
	bool isTopicRestricted() const;
	size_t getUserLimit() const;
	const std::vector<Client*> &getMembers() const;

	// Setters
	void setTopic(const std::string &topic);
	void setKey(const std::string &key);
	void setInviteOnly(bool invite);
	void setTopicRestricted(bool restricted);
	void setUserLimit(size_t limit);

	// Fonctions utilitaires
	bool isMember(const Client *client) const; // Vérifie si un client est déjà membre du canal
};

#endif

