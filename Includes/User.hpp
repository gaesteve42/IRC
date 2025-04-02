/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:29:00 by gaesteve          #+#    #+#             */
/*   Updated: 2025/03/31 17:31:56 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

// Classe représentant un utilisateur connecté au serveur IRC
class Client
{
private:
	std::string nickname;   // Pseudonyme du client affiché dans les salons IRC
	std::string username;   // Nom réel du client (login système ou identifiant unique)
	std::string hostname;   // Adresse IP ou nom d'hôte du client
	bool isOperator;        // Indique si l'utilisateur a des droits spéciaux (opérateur)

public:
	// Constructeurs
	Client(); // Constructeur par défaut
	Client(const std::string &nick, const std::string &user, const std::string &host); // Constructeur paramétré

	// Getters (pour accéder aux attributs privés)
	std::string getNickname() const;
	std::string getUsername() const;
	std::string getHostname() const;
	bool getIsOperator() const;

	// Setters (pour modifier les attributs privés)
	void setNickname(const std::string &nick);
	void setUsername(const std::string &user);
	void setHostname(const std::string &host);
	void setOperator(bool op);
};

#endif

