/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 15:05:58 by yonieva           #+#    #+#             */
/*   Updated: 2025/04/29 18:26:11 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "../Includes/Include.hpp"

class Server
{
private:
	int _serverSocket; // Socket principal du serveur
	int _port; // Port sur lequel on écoute
	std::string _pass; // password pour se connecter
	std::vector<struct pollfd> _pollFds; // Liste des sockets surveillés avec poll()

	void handleNewConnection(); // Gère les nouvelles connexions
	void handleClientMessage(int clientFd); // Gère les messages reçus des clients
	void removeClient(int clientFd); // Supprime un client qui s'est déconnecté
	void processParsedLine(User *user, const std::string &line);
	Parsing parser;
	IRCManager ircManager;

public:
	Server(int port, std::string pass);
	~Server();
	void run(); // Boucle principale du serveur
};

#endif

/*
Création du socket et écoute sur le port donné en argument

Gestion des connexions entrantes (acceptation de nouveaux clients)

Lecture des messages des clients (affichage des messages reçus)

Utilisation de poll() pour gérer plusieurs clients sans blocage
*/

