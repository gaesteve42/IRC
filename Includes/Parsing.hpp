/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yonieva <yonieva@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:16:35 by yonieva           #+#    #+#             */
/*   Updated: 2025/04/29 16:34:42 by yonieva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_HPP
#define PARSING_HPP

#include "../Includes/Include.hpp"

class Parsing
{
	public:
		Parsing();
		~Parsing();

		std::string prefix; // Préfixe (par exemple : :nickname!user@host)
		std::string command; // Commande (par exemple : NICK, USER, PRIVMSG)
		std::string params; // Paramètres (tout avant ":")
		std::string suffix; // Suffixe (tout après ":")

		//void parseCommand(const std::string &message);
		void parseSingleCommand(const std::string &message);
		bool preparePRIVMSG(const std::string& params, const std::string& suffix, std::string& target, std::string& message);
		bool prepareMODE(const std::string &params, std::string &channelName, std::string &modeStr, std::vector<std::string> &modeParams);
		bool prepareKICK(const std::string &params, const std::string &suffix, std::string &channel, std::string &target, std::string &reason);
		bool prepareINVITE(const std::string &params, std::string &channel, std::string &target);
		bool prepareTOPIC(const std::string &params, std::string &channel, std::string &topic);
};

#endif




