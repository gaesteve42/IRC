/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:16:35 by yonieva           #+#    #+#             */
/*   Updated: 2025/04/03 16:59:40 by gaesteve         ###   ########.fr       */
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

        std::string prefix;   // Préfixe (par exemple : :nickname!user@host)
        std::string command;  // Commande (par exemple : NICK, USER, PRIVMSG)
        std::string params;   // Paramètres (tout avant ":")
        std::string suffix;   // Suffixe (tout après ":")

        void parseCommand(const std::string &message);
};

#endif




