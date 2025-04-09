/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Include.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 15:29:54 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/09 21:55:12 by gaesteve         ###   ########.fr       */
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
#include <csignal>
#include <cerrno>

// === Headers du projet ===
#include "User.hpp"
#include "Channel.hpp"
#include "IRCManager.hpp"
#include "Parsing.hpp"
#include "Server.hpp"

// Format standard                        ":server_name CODE nickname :message\r\n"

#define SERVER_NAME "ircserv"

// ========================== 🔴 ERREURS ==========================

#define ERR_NEEDMOREPARAMS(cmd)           (std::string("461 ") + cmd + " :Not enough parameters\r\n")
#define ERR_NOSUCHNICK(nick)              (std::string("401 ") + nick + " :No such nick/channel\r\n")
#define ERR_NOSUCHCHANNEL(chan)           (std::string("403 ") + chan + " :No such channel\r\n")
#define ERR_CANNOTSENDTOCHAN(chan)        (std::string("404 ") + chan + " :Cannot send to channel\r\n")
#define ERR_NOTONCHANNEL(chan)            (std::string("442 ") + chan + " :You're not on that channel\r\n")
#define ERR_USERNOTINCHANNEL(nick, chan)  (std::string("441 ") + nick + " " + chan + " :They aren't on that channel\r\n")
#define ERR_UNKNOWNCOMMAND(cmd)           (std::string("421 ") + cmd + " :Unknown command\r\n")
#define ERR_NOTREGISTERED()               (std::string("451 :You have not registered\r\n"))
#define ERR_ERRONEUSNICKNAME(nick)        (std::string("432 ") + nick + " :Erroneous nickname\r\n")
#define ERR_NICKNAMEINUSE(nick)           (std::string("433 ") + nick + " :Nickname is already in use\r\n")
#define ERR_CHANNELISFULL(chan)           (std::string("471 ") + chan + " :Cannot join channel (+l)\r\n")
#define ERR_INVITEONLYCHAN(chan)          (std::string("473 ") + chan + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANNELKEY(chan)           (std::string("475 ") + chan + " :Cannot join channel (+k)\r\n")
#define ERR_CHANOPRIVSNEEDED(chan)        (std::string("482 ") + chan + " :You're not channel operator\r\n")
#define ERR_ALREADYREGISTRED()            (std::string("462 :You may not register again\r\n"))
#define ERR_PASSWDMISMATCH()              (std::string("464 :Password incorrect\r\n"))
#define ERR_UNKNOWNMODE(mode)             (std::string("472 ") + mode + " :is unknown mode char to me\r\n")
#define ERR_USERONCHANNEL(nick, chan)     (std::string("443 ") + nick + " " + chan + " :is already on channel\r\n")
#define ERR_INVALIDMODEPARAM(chan, mode)  (std::string("696 ") + chan + " " + mode + " :Invalid mode parameter\r\n")

// ========================== 🟢 RÉPONSES ==========================

#define RPL_TOPIC(chan, topic)            (std::string("332 ") + chan + " :" + topic + "\r\n")
#define RPL_NOTOPIC(chan)                 (std::string("331 ") + chan + " :No topic is set\r\n")

inline std::string RPL_NAMREPLY(const std::string& nick, const std::string& symbol, const std::string& channel, const std::string& names)
{
	return ":" + std::string(SERVER_NAME) + " 353 " + nick + " " + symbol + " " + channel + " :" + names + "\r\n";
}
inline std::string RPL_ENDOFNAMES(const std::string& nick, const std::string& channel)
{
	return ":" + std::string(SERVER_NAME) + " 366 " + nick + " " + channel + " :End of /NAMES list.\r\n";
}


#endif

