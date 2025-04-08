/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Include.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yonieva <yonieva@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 15:29:54 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/08 17:29:07 by yonieva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INCLUDE_HPP
#define INCLUDE_HPP

// === Includes principaux ===
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>

// === Headers du projet ===
#include "User.hpp"
#include "Channel.hpp"
#include "IRCManager.hpp"
#include "Parsing.hpp"
#include "Server.hpp"

// Format standard : ":server_name CODE nickname :message\r\n"

// 🔴 ERREURS
#define ERR_NEEDMOREPARAMS(cmd)            std::string("461 ") + cmd + " :Not enough parameters\r\n"                      // Paramètres manquants
#define ERR_NOSUCHNICK(nick)               std::string("401 ") + nick + " :No such nick/channel\r\n"                      // Nickname ou canal inexistant
#define ERR_NOSUCHCHANNEL(chan)            std::string("403 ") + chan + " :No such channel\r\n"                           // Canal inexistant
#define ERR_CANNOTSENDTOCHAN(chan)         std::string("404 ") + chan + " :Cannot send to channel\r\n"                    // impossible d envoyer
#define ERR_NOTONCHANNEL(chan)             std::string("442 ") + chan + " :You're not on that channel\r\n"                // Tu n'es pas dans ce canal
#define ERR_USERNOTINCHANNEL(nick, chan)   std::string("441 ") + nick + " " + chan + " :They aren't on that channel\r\n"  // L'utilisateur n'est pas dans le canal
#define ERR_UNKNOWNCOMMAND(cmd)            std::string("421 ") + cmd + " :Unknown command\r\n"                            // Commande inconnue
#define ERR_NOTREGISTERED()                std::string("451 ") + ":You have not registered\r\n"                                // Pas authentifié
#define ERR_ERRONEUSNICKNAME(nick)         std::string("432 ") + nick + " :Erroneous nickname\r\n"                        // Nickname invalide
#define ERR_NICKNAMEINUSE(nick)            std::string("433 ") + nick + " :Nickname is already in use\r\n"                // Nickname déjà utilisé
#define ERR_CHANNELISFULL(chan)            std::string("471 ") + chan + " :Cannot join channel (+l)\r\n"                  // Limite atteinte
#define ERR_INVITEONLYCHAN(chan)           std::string("473 ") + chan + " :Cannot join channel (+i)\r\n"                  // Canal en +i ( invitation uniquement )
#define ERR_BADCHANNELKEY(chan)            std::string("475 ") + chan + " :Cannot join channel (+k)\r\n"                  // Mauvais mot de passe
#define ERR_CHANOPRIVSNEEDED(chan)         std::string("482 ") + chan + " :You're not channel operator\r\n"               // Non-opérateur
#define ERR_ALREADYREGISTRED()             std::string("462 ") + ":You may not reregister\r\n"                                 // Utilisateur déjà authentifié
#define ERR_PASSWDMISMATCH()               std::string("464 ") + ":Password incorrect\r\n"                                     // Mot de passe incorrect (si PASS est requis)
#define ERR_UNKNOWNMODE(mode)             std::string("472 ") + mode + " :is unknown mode char to me\r\n"                 // Mode inconnu ou invalide
#define ERR_USERONCHANNEL(nick, chan)      std::string("443 ") + nick + " " + chan + " :is already on channel\r\n"        // L'utilisateur est déjà dans le canal
#define ERR_INVALIDMODEPARAM(chan, mode)   std::string("696 ") + chan + " " + mode + " :Invalid mode parameter\r\n"       // Paramètre de mode invalide

// 🟢 RÉPONSES
#define RPL_TOPIC(chan, topic)             std::string("332 ") + chan + " :" + topic + "\r\n"                // Aucun sujet défini
#define RPL_NOTOPIC(chan)                  std::string("331 ") + chan + " :No topic is set\r\n"              // Sujet actuel du canal
#define RPL_NAMREPLY(chan, names)         std::string("353 = ") + chan + " :" + names + "\r\n"               // Liste des utilisateurs d'un canal
#define RPL_ENDOFNAMES(chan)              std::string("366 ") + chan + " :End of /NAMES list\r\n"            // Fin de la liste des utilisateurs d’un canal

#endif

