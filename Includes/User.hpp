/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:29:00 by gaesteve          #+#    #+#             */
/*   Updated: 2025/05/09 23:48:03 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include "../Includes/Include.hpp"

class User
{
private:
	int fd;
	std::string nickname; // Pseudonyme de l'utilisateur affiché dans les salons IRC
	std::string username; // Nom réel de l'utilisateur (login système ou identifiant unique)
	std::string hostname; // Adresse IP ou nom d'hôte de l'utilisateur
	bool Password_is_ok; // Indique si le password est ok
	bool authenticated; // indique si le User est authentifié

public:
	// Constructeurs
	User();
	User(int fd);
	// Getters
	std::string recvBuffer;
	bool passMess;
	std::string getNickname() const;
	std::string getUsername() const;
	std::string getHostname() const;
	int getFd() const;
	bool hasProvidedPassword() const;
	bool isAuthenticated() const;
	//Setters
	void setNickname(const std::string &nick);
	void setUsername(const std::string &user);
	void setHostname(const std::string &host);
	void setAuthenticated(bool auth);
	void setHasProvidedPassword(bool val);
};

#endif

