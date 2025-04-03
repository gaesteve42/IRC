/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Include.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 15:29:54 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/03 17:35:05 by gaesteve         ###   ########.fr       */
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
#define ERR_NEEDMOREPARAMS(cmd)           "461 " + cmd + " :Not enough parameters\r\n"                      // Paramètres manquants
#define ERR_NOSUCHNICK(nick)              "401 " + nick + " :No such nick/channel\r\n"                      // Nickname ou canal inexistant
#define ERR_NOSUCHCHANNEL(chan)           "403 " + chan + " :No such channel\r\n"                           // Canal inexistant
#define ERR_CANNOTSENDTOCHAN(chan)        "404 " + chan + " :Cannot send to channel\r\n"                    // impossible d envoyer
#define ERR_NOTONCHANNEL(chan)            "442 " + chan + " :You're not on that channel\r\n"                // Tu n'es pas dans ce canal
#define ERR_USERNOTINCHANNEL(nick, chan)  "441 " + nick + " " + chan + " :They aren't on that channel\r\n"  // L'utilisateur n'est pas dans le canal
#define ERR_UNKNOWNCOMMAND(cmd)           "421 " + cmd + " :Unknown command\r\n"                            // Commande inconnue
#define ERR_NOTREGISTERED()               "451 :You have not registered\r\n"                                // Pas authentifié
#define ERR_ERRONEUSNICKNAME(nick)        "432 " + nick + " :Erroneous nickname\r\n"                        // Nickname invalide
#define ERR_NICKNAMEINUSE(nick)           "433 " + nick + " :Nickname is already in use\r\n"                // Nickname déjà utilisé
#define ERR_CHANNELISFULL(chan)           "471 " + chan + " :Cannot join channel (+l)\r\n"                  // Limite atteinte
#define ERR_INVITEONLYCHAN(chan)          "473 " + chan + " :Cannot join channel (+i)\r\n"                  // Canal en +i ( invitation uniquement )
#define ERR_BADCHANNELKEY(chan)           "475 " + chan + " :Cannot join channel (+k)\r\n"                  // Mauvais mot de passe
#define ERR_CHANOPRIVSNEEDED(chan)        "482 " + chan + " :You're not channel operator\r\n"               // Non-opérateur
#define ERR_ALREADYREGISTRED()            "462 :You may not reregister\r\n"                                 // Utilisateur déjà authentifié
#define ERR_PASSWDMISMATCH()              "464 :Password incorrect\r\n"                                     // Mot de passe incorrect (si PASS est requis)
#define ERR_UNKNOWNMODE(mode)            "472 " + mode + " :is unknown mode char to me\r\n"                 // Mode inconnu ou invalide
#define ERR_USERONCHANNEL(nick, chan)     "443 " + nick + " " + chan + " :is already on channel\r\n"        // L'utilisateur est déjà dans le canal
#define ERR_INVALIDMODEPARAM(chan, mode)  "696 " + chan + " " + mode + " :Invalid mode parameter\r\n"       // Paramètre de mode invalide

// 🟢 RÉPONSES
#define RPL_TOPIC(chan, topic)            "332 " + chan + " :" + topic + "\r\n"                // Aucun sujet défini
#define RPL_NOTOPIC(chan)                 "331 " + chan + " :No topic is set\r\n"              // Sujet actuel du canal
#define RPL_NAMREPLY(chan, names)        "353 = " + chan + " :" + names + "\r\n"               // Liste des utilisateurs d'un canal
#define RPL_ENDOFNAMES(chan)             "366 " + chan + " :End of /NAMES list\r\n"            // Fin de la liste des utilisateurs d’un canal

#endif

