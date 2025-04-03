/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yonieva <yonieva@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:17:03 by yonieva           #+#    #+#             */
/*   Updated: 2025/04/03 16:47:09 by yonieva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Parsing.hpp"

Parsing::Parsing() {}

Parsing::~Parsing() {}

void Parsing::parseCommand(const std::string &message) 
{
    // Retirer les caractères de fin de ligne éventuels
    std::string trimmedMessage = message;
    if (trimmedMessage.back() == '\n')
        trimmedMessage.pop_back();  // Retirer '\n'
    if (trimmedMessage.back() == '\r')
        trimmedMessage.pop_back();  // Retirer '\r'

        
    // Initialiser toutes les variables membres à vide
    prefix.clear();
    command.clear();
    params.clear();
    suffix.clear();

    std::istringstream stream(message);
    std::string temp;

    // Partie 1 : Préfixe (si présent)
    if (message[0] == ':') 
    {
        size_t spacePos = message.find(' ');
        if (spacePos != std::string::npos) 
        {
            prefix = message.substr(1, spacePos - 1);
            message = message.substr(spacePos + 1);  // Le message sans le préfixe
        }
    }

    // Partie 2 : Commande (jusqu'au premier espace)
    size_t spacePos = message.find(' ');
    if (spacePos != std::string::npos) 
    {
        command = message.substr(0, spacePos);
        message = message.substr(spacePos + 1);
    } 
    else 
    {
        command = message;
        message.clear();
    }

    // Partie 3 : Paramètres (tout avant ":")
    size_t colonPos = message.find(':');
    if (colonPos != std::string::npos) 
    {
        params = message.substr(0, colonPos);
        suffix = message.substr(colonPos + 1);  // Le suffixe après ":"
    } 
    else 
    {
        params = message;
    }
}

/*POUR TESTER DANS UN MAIN
Parsing parser;
parser.parseCommand(message);

std::cout << "Préfixe : " << parser.prefix << std::endl;
std::cout << "Commande : " << parser.command << std::endl;
std::cout << "Paramètres : " << parser.params << std::endl;
std::cout << "Suffixe : " << parser.suffix << std::endl;
*/


//1 prefix --> :(index[0]) XXXX 
//2 command --> ex KICK
//3 parametres --> tout ce qu il y a avant ":" ou "/n ou /r/n" 
//4 suffix --> tout ce qu il y a apres ":"