/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yonieva <yonieva@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:16:35 by yonieva           #+#    #+#             */
/*   Updated: 2025/04/03 16:54:40 by yonieva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_HPP
#define PARSING_HPP

#include <iostream>
#include <sstream>

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




