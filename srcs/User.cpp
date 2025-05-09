/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:28:53 by gaesteve          #+#    #+#             */
/*   Updated: 2025/05/09 23:48:08 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Include.hpp"

// Constructeur par défaut
User::User() : fd(-1), nickname(""), username(""), hostname(""), Password_is_ok(false), authenticated(false), passMess(true) {}

//Constructeur avec fd
User::User(int fd) : fd(fd), nickname(""), username(""), hostname(""), Password_is_ok(false), authenticated(false), passMess(true) {}

// ===== Getters (accès en lecture aux attributs privés) =====
std::string User::getNickname() const { return nickname; }
std::string User::getUsername() const { return username; }
std::string User::getHostname() const { return hostname; }
bool User::isAuthenticated() const { return authenticated; }
int User::getFd() const { return fd; }
bool User::hasProvidedPassword() const { return Password_is_ok; }

// ===== Setters (modification des attributs privés) =====
void User::setNickname(const std::string &nick) { nickname = nick; }
void User::setUsername(const std::string &user) { username = user; }
void User::setHostname(const std::string &host) { hostname = host; }
void User::setAuthenticated(bool auth) { authenticated = auth; }
void User::setHasProvidedPassword(bool val) { Password_is_ok = val; }
