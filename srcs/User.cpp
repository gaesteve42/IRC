/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:28:53 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/03 16:36:38 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Include.hpp"

// Constructeur par défaut
User::User() : nickname(""), username(""), hostname(""), isOperator(false), authenticated(false) {}

// Constructeur paramétré (permet d'initialiser immédiatement les attributs)
User::User(const std::string &nick, const std::string &user, const std::string &host)
	: nickname(nick), username(user), hostname(host), isOperator(false) {}

// ===== Getters (accès en lecture aux attributs privés) =====
std::string User::getNickname() const { return nickname; }
std::string User::getUsername() const { return username; }
std::string User::getHostname() const { return hostname; }
bool User::getIsOperator() const { return isOperator; }
bool User::isAuthenticated() const { return authenticated; }

// ===== Setters (modification des attributs privés) =====
void User::setNickname(const std::string &nick) { nickname = nick; }
void User::setUsername(const std::string &user) { username = user; }
void User::setHostname(const std::string &host) { hostname = host; }
void User::setOperator(bool op) { isOperator = op; }
void User::setAuthenticated(bool auth) { authenticated = auth; }
