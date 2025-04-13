/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:17:03 by yonieva           #+#    #+#             */
/*   Updated: 2025/04/13 15:36:01 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Include.hpp"

Parsing::Parsing() {}

Parsing::~Parsing() {}


void Parsing::parseCommand(const std::string &message)
{
    // Créer un flux à partir du message pour découper en plusieurs commandes
    std::istringstream stream(message);
    std::string line;

    // Découper le message en plusieurs commandes séparées par "\r\n" ou "\n"
    while (std::getline(stream, line, '\n')) // getline retire le \n
    {
        // Nettoyer les caractères de fin de ligne '\r' ou '\n' (ceux qui peuvent rester en fin de ligne)
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);  // Retirer le '\r' si présent

        // Traiter chaque ligne (commande) après nettoyage
        parseSingleCommand(line);
    }
}

void Parsing::parseSingleCommand(const std::string &msg)
{
	// Copie locale modifiable du message
	std::string message = msg;

	// Retirer les caractères de fin de ligne éventuels
	if (!message.empty() && message[message.size() - 1] == '\n')
		message.erase(message.size() - 1);
	if (!message.empty() && message[message.size() - 1] == '\r')
		message.erase(message.size() - 1);

	// Initialiser toutes les variables membres
	prefix = "";
	command = "";
	params = "";
	suffix = "";

	std::string temp;

	// Partie 1 : Préfixe (si présent)
	if (!message.empty() && message[0] == ':')
	{
		size_t spacePos = message.find(' ');
		if (spacePos != std::string::npos)
		{
			prefix = message.substr(1, spacePos - 1);
			message = message.substr(spacePos + 1); // OK : la copie est modifiable
		}
	}

	// Partie 2 : Commande
	size_t spacePos = message.find(' ');
	if (spacePos != std::string::npos)
	{
		command = message.substr(0, spacePos);
		message = message.substr(spacePos + 1);
	}
	else
	{
		command = message;
		message = ""; // pour C++98
	}

	// Partie 3 : Paramètres et suffixe
	size_t colonPos = message.find(':');
	if (colonPos != std::string::npos)
	{
		params = message.substr(0, colonPos);
		suffix = message.substr(colonPos + 1);
	}
	else
	{
		params = message;
	}
}

// Préparer la commande PRIVMSG (séparation du canal et du message)
bool Parsing::preparePRIVMSG(const std::string& params, const std::string& suffix, std::string& target, std::string& message)
{
	if (params.empty() || suffix.empty())
		return false;
	target = params;
	// Nettoyage des espaces à droite
	while (!target.empty() && target[target.size() - 1] == ' ')
		target.erase(target.size() - 1);
	message = suffix;
	return true;
}

bool Parsing::prepareMODE(const std::string &params, std::string &channelName, std::string &modes, std::string &param)
{
	// 1) On parse d’abord ce qu’on a dans params (comme avant)
	size_t spacePos = params.find(' ');
	if (spacePos == std::string::npos)
		return false;
	// channelName = "#test"
	channelName = params.substr(0, spacePos);
	// modeAndParam = "+il 10" (ou juste "+il", etc.)
	std::string modeAndParam = params.substr(spacePos + 1);
	size_t modePos = modeAndParam.find(' ');
	if (modePos != std::string::npos)
	{
		// modes = "+il"
		modes = modeAndParam.substr(0, modePos);
		// param = "10"
		param = modeAndParam.substr(modePos + 1);
	}
	else
	{
		// pas de param -> param = ""
		modes = modeAndParam;
		param = "";
	}
	// 2) Fallback si jamais c’est vide et qu’on a un suffix
	// (cas ou parseSingleCommand a mis "10" dans suffix par ex)
	// ou s’il manque un param pour +k, +l, +o
	if (!suffix.empty())
	{
		// on s’assure qu’on ne double pas
		if (!param.empty())
		param += " ";
		param += suffix;
	}
	return !channelName.empty();
}

bool Parsing::prepareKICK(const std::string &params, const std::string &suffix,
	std::string &channel, std::string &target, std::string &reason)
{
	size_t firstSpace = params.find(' ');
	if (firstSpace == std::string::npos)
		return false;
	channel = params.substr(0, firstSpace);
	size_t secondSpace = params.find(' ', firstSpace + 1);
	if (secondSpace == std::string::npos)
		target = params.substr(firstSpace + 1);
	else
		target = params.substr(firstSpace + 1, secondSpace - firstSpace - 1);
	reason = suffix;
	while (!reason.empty() && reason[0] == ' ')
		reason.erase(0, 1);
	while (!reason.empty() && reason[reason.size() - 1] == ' ')
		reason.erase(reason.size() - 1);
	if (!reason.empty() && reason[0] == ':')
		reason.erase(0, 1);
	while (!reason.empty() && reason[0] == ' ')
		reason.erase(0, 1);
	return !channel.empty() && !target.empty();
}


bool Parsing::prepareINVITE(const std::string &params, std::string &channel, std::string &target)
{
    size_t spacePos = params.find(' ');
    if (spacePos == std::string::npos) return false;

    target = params.substr(0, spacePos);
    channel = params.substr(spacePos + 1);

    return true;
}

bool Parsing::prepareTOPIC(const std::string &params, std::string &channel, std::string &topic)
{
	size_t spacePos = params.find(' ');
	if (spacePos == std::string::npos)
	{
		channel = params;
		topic = "";  // Pas de sujet, on veut juste consulter
	}
	else
	{
		channel = params.substr(0, spacePos);
		topic = params.substr(spacePos + 1);
	}
	return true;
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
