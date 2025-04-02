/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:28:53 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/02 11:58:45 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/User.hpp"

// Constructeur par défaut
Client::Client() : nickname(""), username(""), hostname(""), isOperator(false) {}

// Constructeur paramétré (permet d'initialiser immédiatement les attributs)
Client::Client(const std::string &nick, const std::string &user, const std::string &host)
	: nickname(nick), username(user), hostname(host), isOperator(false) {}

// ===== Getters (accès en lecture aux attributs privés) =====
std::string Client::getNickname() const { return nickname; }
std::string Client::getUsername() const { return username; }
std::string Client::getHostname() const { return hostname; }
bool Client::getIsOperator() const { return isOperator; }

// ===== Setters (modification des attributs privés) =====
void Client::setNickname(const std::string &nick) { nickname = nick; }
void Client::setUsername(const std::string &user) { username = user; }
void Client::setHostname(const std::string &host) { hostname = host; }
void Client::setOperator(bool op) { isOperator = op; }

