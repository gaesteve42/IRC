/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:29:00 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/03 16:36:24 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include "../Includes/Include.hpp"

class User
{
private:
	std::string nickname;   // Pseudonyme de l'utilisateur affiché dans les salons IRC
	std::string username;   // Nom réel de l'utilisateur (login système ou identifiant unique)
	std::string hostname;   // Adresse IP ou nom d'hôte de l'utilisateur
	bool isOperator;        // Indique si l'utilisateur a des droits spéciaux (opérateur)
	bool authenticated;     // indique si le User est authentifié

public:
	// Constructeurs
	User(); // Constructeur par défaut
	User(const std::string &nick, const std::string &user, const std::string &host); // Constructeur paramétré

	// Getters (pour accéder aux attributs privés)
	std::string getNickname() const;
	std::string getUsername() const;
	std::string getHostname() const;
	bool getIsOperator() const;
	bool isAuthenticated() const;

	// Setters (pour modifier les attributs privés)
	void setNickname(const std::string &nick);
	void setUsername(const std::string &user);
	void setHostname(const std::string &host);
	void setOperator(bool op);
	void setAuthenticated(bool auth);
};

#endif

