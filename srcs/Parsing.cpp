/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yonieva <yonieva@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:17:03 by yonieva           #+#    #+#             */
/*   Updated: 2025/04/02 16:32:06 by yonieva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Parsing.hpp"

Parsing::Parsing() {}

Parsing::~Parsing() {}

std::vector<std::string> Parsing::parseCommand(const std::string &message) 
{
    std::vector<std::string> tokens;
    std::istringstream stream(message);
    std::string token;

    while (stream >> token) 
        tokens.push_back(token);

    return tokens;
}

/*
Découpe le message reçu en mots séparés par des espaces.
Stocke chaque mot dans un vector<string>.
Permet à Server d’interpréter la commande et de l’envoyer à IRCManager.
*/
