/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Include.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 15:29:54 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/03 17:02:57 by gaesteve         ###   ########.fr       */
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
#define ERR_NEEDMOREPARAMS(cmd)           "461 " + cmd + " :Not enough parameters\r\n"
#define ERR_NOSUCHNICK(nick)              "401 " + nick + " :No such nick/channel\r\n"
#define ERR_NOSUCHCHANNEL(chan)           "403 " + chan + " :No such channel\r\n"
#define ERR_CANNOTSENDTOCHAN(chan)        "404 " + chan + " :Cannot send to channel\r\n"
#define ERR_NOTONCHANNEL(chan)            "442 " + chan + " :You're not on that channel\r\n"
#define ERR_USERNOTINCHANNEL(nick, chan)  "441 " + nick + " " + chan + " :They aren't on that channel\r\n"
#define ERR_UNKNOWNCOMMAND(cmd)           "421 " + cmd + " :Unknown command\r\n"
#define ERR_NOTREGISTERED()               "451 :You have not registered\r\n"
#define ERR_ERRONEUSNICKNAME(nick)        "432 " + nick + " :Erroneous nickname\r\n"
#define ERR_NICKNAMEINUSE(nick)           "433 " + nick + " :Nickname is already in use\r\n"
#define ERR_CHANNELISFULL(chan)           "471 " + chan + " :Cannot join channel (+l)\r\n"
#define ERR_INVITEONLYCHAN(chan)          "473 " + chan + " :Cannot join channel (+i)\r\n"
#define ERR_BADCHANNELKEY(chan)           "475 " + chan + " :Cannot join channel (+k)\r\n"
#define ERR_CHANOPRIVSNEEDED(chan)        "482 " + chan + " :You're not channel operator\r\n"

// 🟢 RÉPONSES
#define RPL_TOPIC(chan, topic)            "332 " + chan + " :" + topic + "\r\n"
#define RPL_NOTOPIC(chan)                 "331 " + chan + " :No topic is set\r\n"

#endif

