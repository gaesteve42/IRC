/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 15:07:45 by yonieva           #+#    #+#             */
/*   Updated: 2025/04/29 18:26:24 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Include.hpp"

Server::Server(int port, std::string pass) : _port(port), _pass(pass)
{
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket < 0)
	{
		std::cerr << "❌Erreur❌ : Impossible de créer le socket" << std::endl;
		exit(EXIT_FAILURE);
	}

	int opt = 1;
	setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(_port);

	if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "❌Erreur❌ : Bind échoué" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (listen(_serverSocket, 10) < 0)
	{
		std::cerr << "❌Erreur❌ : Listen échoué" << std::endl;
		exit(EXIT_FAILURE);
	}

	struct pollfd serverPollFd;
	serverPollFd.fd = _serverSocket;
	serverPollFd.events = POLLIN;
	_pollFds.push_back(serverPollFd);

	std::cout << "✅ Serveur lancé sur le port " << _port << std::endl;
}

Server::~Server()
{
	close(_serverSocket);
	for (size_t i = 0; i < _pollFds.size(); i++)
		close(_pollFds[i].fd);
}

void Server::handleClientMessage(int clientFd)
{
	char buffer[512];
	memset(buffer, 0, sizeof(buffer));

	int bytesReceived = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
	User *user = ircManager.getUser(clientFd);
	if (!user)
		return;

	if (bytesReceived == 0)
	{
		if (user && !user->recvBuffer.empty())
		{
			size_t pos;
			while ((pos = user->recvBuffer.find('\n')) != std::string::npos)
			{
				std::string line = user->recvBuffer.substr(0, pos);
				user->recvBuffer.erase(0, pos + 1);

				if (!line.empty() && line[line.size() - 1] == '\r')
					line.erase(line.size() - 1);

				if (!line.empty())
					processParsedLine(user, line);
			}
			if (!user->recvBuffer.empty())
				processParsedLine(user, user->recvBuffer);
		}

		std::cout << "❌ Déconnexion (FD : " << clientFd << ")" << std::endl;
		removeClient(clientFd);
		return;
	}
	else if (bytesReceived < 0)
	{
		std::cerr << "⚠️ Erreur de recv()" << std::endl;
		return;
	}

	user->recvBuffer += std::string(buffer, bytesReceived);
	std::cout << "📥 RECV (" << bytesReceived << " bytes): \""
				<< std::string(buffer, bytesReceived) << "\"" << std::endl;
	std::cout << "🧪 Buffer actuel pour FD " << clientFd << ": \""
				<< user->recvBuffer << "\"" << std::endl;

	size_t pos;
	while ((pos = user->recvBuffer.find('\n')) != std::string::npos)
	{
		std::string line = user->recvBuffer.substr(0, pos);
		user->recvBuffer.erase(0, pos + 1);

		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		if (!line.empty())
		{
			std::cout << "📤 Traitement d'une ligne complète : \"" << line << "\"" << std::endl;
			processParsedLine(user, line);
		}
	}
}

void Server::handleNewConnection()
{
	struct sockaddr_in clientAddr;
	memset(&clientAddr, 0, sizeof(clientAddr));
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientFd = accept(_serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (clientFd < 0)
	{
		std::cerr << "❌ Erreur : échec de l'acceptation de la connexion" << std::endl;
		return;
	}
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Erreur lors du fcntl (F_SETFL)" << std::endl;
		close(clientFd);
		return;
	}

	struct pollfd clientPollFd;
	clientPollFd.fd = clientFd;
	clientPollFd.events = POLLIN;
	clientPollFd.revents = 0;
	_pollFds.push_back(clientPollFd);
	std::cout << "✅ Nouveau client connecté (FD : " << clientFd << ")" << std::endl;
	ircManager.newUser(clientFd);
}

extern volatile sig_atomic_t running;
void Server::run()
{
	while (running)
	{
		int pollCount = poll(_pollFds.data(), _pollFds.size(), -1);
		if (pollCount < 0)
		{
			if (errno == EINTR)
				break;
			else
			{
				std::cerr << "⚠️❌ Erreur : poll() a échoué" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		for (size_t i = 0; i < _pollFds.size(); i++)
		{
			if (_pollFds[i].revents & POLLIN)
			{
				if (_pollFds[i].fd == _serverSocket)
					handleNewConnection();
				else
					handleClientMessage(_pollFds[i].fd);
			}
		}
	}
}

void Server::removeClient(int clientFd)
{
	close(clientFd);
	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		if (_pollFds[i].fd == clientFd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}

	User *user = ircManager.getUser(clientFd);
	if (user)
	{
		std::map<std::string, Channel*>& channels = ircManager.getChannels();
		std::vector<std::string> joinedChannels;
		for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
		{
			if (it->second->isMember(user))
				joinedChannels.push_back(it->first);
		}
		for (size_t i = 0; i < joinedChannels.size(); ++i)
		{
			ircManager.partCommand(clientFd, joinedChannels[i], "");
		}
	}
	ircManager.removeUser(clientFd);
}

void Server::processParsedLine(User *user, const std::string &line)
{
	std::cout << "🧠 Appel de processParsedLine() avec : \"" << line << "\"" << std::endl;

	Parsing parsedMessage;
	parsedMessage.parseSingleCommand(line);
	int clientFd = user->getFd();

	std::cout << ">> Client Fd " << clientFd << std::endl;
	std::cout << ">> Prefix : " << parsedMessage.prefix << std::endl;
	std::cout << ">> Commande : " << parsedMessage.command << std::endl;
	std::cout << ">> Parametres : " << parsedMessage.params << std::endl;
	std::cout << ">> Suffix : " << parsedMessage.suffix << std::endl << std::endl;

	if (!user->hasProvidedPassword())
	{
		if (parsedMessage.command == "PASS")
		{
			if (parsedMessage.params == _pass)
			{
				user->setHasProvidedPassword(true);
				std::string msg = ":ircserv NOTICE * :Mot de passe accepté par le serveur\r\n";
				send(clientFd, msg.c_str(), msg.length(), 0);
			}
			else
			{
				std::string err = "ERROR :Mot de passe incorrect, deconnexion du serveur...\r\n";
				send(clientFd, err.c_str(), err.length(), 0);
				removeClient(clientFd);
			}
		}
		else
		{
			if (user->passMess)
			{
				std::string err = "ERROR :Mot de passe requis\r\n";
				send(clientFd, err.c_str(), err.length(), 0);
				user->passMess = false;
			}
		}
		return;
	}

	if ((parsedMessage.command == "NICK" || parsedMessage.command == "USER" ||
		 parsedMessage.command == "JOIN" || parsedMessage.command == "PART") &&
		 parsedMessage.params.empty())
	{
		std::string errorMsg = ERR_NEEDMOREPARAMS(parsedMessage.command);
		send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);
		return;
	}

	if (parsedMessage.command == "NICK")
		ircManager.nickCommand(clientFd, parsedMessage.params);
	else if (parsedMessage.command == "USER")
		ircManager.userCommand(clientFd, parsedMessage.params);
	else if (parsedMessage.command == "JOIN")
		ircManager.joinCommand(clientFd, parsedMessage.params);
	else if (parsedMessage.command == "PART")
	{
		std::string channelName;
		std::istringstream iss(parsedMessage.params);
		iss >> channelName;
		ircManager.partCommand(clientFd, channelName, parsedMessage.suffix);
	}
	else if (parsedMessage.command == "PRIVMSG")
	{
		std::string channel, message;
		if (parsedMessage.preparePRIVMSG(parsedMessage.params, parsedMessage.suffix, channel, message))
			ircManager.privmsgCommand(clientFd, channel, message);
		else
		{
			std::string errorMsg = ERR_NEEDMOREPARAMS(parsedMessage.command);
			send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);
		}
	}
	else if (parsedMessage.command == "MODE")
	{
		std::string channelName, modeStr;
		std::vector<std::string> modeParams;
		if (parsedMessage.prepareMODE(parsedMessage.params, channelName, modeStr, modeParams))
			ircManager.modeCommand(clientFd, channelName, modeStr, modeParams);
		else
		{
			std::string errorMsg = ERR_NEEDMOREPARAMS(parsedMessage.command);
			send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);
		}
	}
	else if (parsedMessage.command == "KICK")
	{
		std::string channel, target, reason;
		if (parsedMessage.prepareKICK(parsedMessage.params, parsedMessage.suffix, channel, target, reason))
			ircManager.kickCommand(clientFd, channel, target, reason);
		else
		{
			std::string err = ERR_NEEDMOREPARAMS("KICK");
			send(clientFd, err.c_str(), err.length(), 0);
		}
	}
	else if (parsedMessage.command == "INVITE")
	{
		std::string channel, target;
		if (parsedMessage.prepareINVITE(parsedMessage.params, channel, target))
			ircManager.inviteCommand(clientFd, channel, target);
		else
		{
			std::string err = ERR_NEEDMOREPARAMS("INVITE");
			send(clientFd, err.c_str(), err.length(), 0);
		}
	}
	else if (parsedMessage.command == "TOPIC")
	{
		std::string channelName;
		std::istringstream iss(parsedMessage.params);
		if (!(iss >> channelName))
		{
			std::string err = ERR_NEEDMOREPARAMS("TOPIC");
			send(clientFd, err.c_str(), err.length(), 0);
			return;
		}
		ircManager.topicCommand(clientFd, channelName, parsedMessage.suffix);
	}
	else if (parsedMessage.command != "WHO")
	{
		std::string errorMsg = ERR_UNKNOWNCOMMAND(parsedMessage.command);
		send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);
	}
}
// //--------------------------------------------------------------------CONSTUCTION DU SERVEUR-----------------------------------------------------------------
// //-----------------------------------------------------------------------------------------------------------------------------------------------------------
// // Constructeur du serveur
// Server::Server(int port, std::string pass) : _port(port), _pass(pass)
// {
// 	// Création du socket du serveur
// 	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
// 	if (_serverSocket < 0)
// 	{
// 		std::cerr << "❌Erreur❌ : Impossible de créer le socket" << std::endl;
// 		exit(EXIT_FAILURE);
// 	}

// 	// Permet de réutiliser rapidement l'adresse et le port après un arrêt
// 	int opt = 1;
// 	setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

// 	// Configuration de l'adresse du serveur
// 	struct sockaddr_in serverAddr;
// 	memset(&serverAddr, 0, sizeof(serverAddr));
// 	serverAddr.sin_family = AF_INET;
// 	serverAddr.sin_addr.s_addr = INADDR_ANY;
// 	serverAddr.sin_port = htons(_port);

// 	// Attacher le socket à l'adresse et au port
// 	if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
// 	{
// 		std::cerr << "❌Erreur❌ : Bind échoué" << std::endl;
// 		exit(EXIT_FAILURE);
// 	}

// 	// Écoute des connexions entrantes
// 	if (listen(_serverSocket, 10) < 0)
// 	{
// 		std::cerr << "❌Erreur❌ : Listen échoué" << std::endl;
// 		exit(EXIT_FAILURE);
// 	}

// 	// Ajouter le socket du serveur à poll()
// 	struct pollfd serverPollFd;
// 	serverPollFd.fd = _serverSocket;
// 	serverPollFd.events = POLLIN;
// 	_pollFds.push_back(serverPollFd);

// 	std::cout << "✅ Serveur lancé sur le port " << _port << std::endl;
// }

// //----------------------------------------------------------------------------------------

// // Destructeur : ferme le socket principal et tous les clients
// Server::~Server()
// {
// 	close(_serverSocket);
// 	for (size_t i = 0; i < _pollFds.size(); i++)
// 	{
// 		close(_pollFds[i].fd);
// 	}
// }

// //--------------------------------------------------------------------------------------------------------------------------------------------------------
// //--------------------------------------------------------------------------------------------------------------------------------------------------------





// //--------------------------------------------------------------------SERVEUR EN ROUTE--------------------------------------------------------------------
// //--------------------------------------------------------------------------------------------------------------------------------------------------------
// void Server::handleClientMessage(int clientFd)
// {
//     char buffer[512];
//     memset(buffer, 0, sizeof(buffer));

//     int bytesReceived = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

//     User *user = ircManager.getUser(clientFd);
//     if (!user)
//         return;



//     if (bytesReceived == 0)
//     {
//         User *user = ircManager.getUser(clientFd);
//         if (!user || user->recvBuffer.empty())
//         {
//             std::cout << "❌ Déconnexion (FD : " << clientFd << ")" << std::endl;
//             removeClient(clientFd);
//             ircManager.removeUser(clientFd);
//         }
//         return; // on ne traite pas maintenant, on attend peut-être encore une fin de commande
//     }
//     else if (bytesReceived < 0)
//     {
//         std::cerr << "⚠️ Erreur de recv()" << std::endl;
//         return;
//     }

//     // Ajouter à son buffer perso
//     user->recvBuffer += std::string(buffer, bytesReceived);


//     size_t pos;
//     while ((pos = user->recvBuffer.find('\n')) != std::string::npos)
//     {
//         std::string line = user->recvBuffer.substr(0, pos);
//         user->recvBuffer.erase(0, pos + 1);

//         // Nettoyage du \r final éventuel
//         if (!line.empty() && line[line.size() - 1] == '\r')
//             line.erase(line.size() - 1);

//         if (line.empty())
//             continue;

//         Parsing parsedMessage;
//         parsedMessage.parseSingleCommand(line);

//         std::cout << ">> Client Fd " << clientFd << std::endl;
//         std::cout << ">> Prefix : " << parsedMessage.prefix << std::endl;
//         std::cout << ">> Commande : " << parsedMessage.command << std::endl;
//         std::cout << ">> Parametres : " << parsedMessage.params << std::endl;
//         std::cout << ">> Suffix : " << parsedMessage.suffix << std::endl << std::endl;

//         // Check mot de passe
//         if (!user->hasProvidedPassword())
//         {
//             if (parsedMessage.command == "PASS")
//             {
//                 if (parsedMessage.params == _pass)
//                 {
//                     user->setHasProvidedPassword(true);
//                     std::cout << "✅ Mot de passe accepté pour FD " << clientFd << std::endl;
//                     std::string msg = ":ircserv NOTICE * :Mot de passe accepté par le serveur\r\n";
//                     send(clientFd, msg.c_str(), msg.length(), 0);
//                     continue;
//                 }
//                 else
//                 {
//                     std::cout << "❌ Mauvais mot de passe pour FD " << clientFd << std::endl;
//                     std::string err = "ERROR :Mot de passe incorrect, deconnexion du serveur...\r\n";
//                     send(clientFd, err.c_str(), err.length(), 0);
//                     removeClient(clientFd);
//                     ircManager.removeUser(clientFd);
//                     return;
//                 }
//             }
//             else
//             {
//                 if (user->passMess == 1)
//                 {
//                     std::string err = "ERROR :Mot de passe requis\r\n";
//                     send(clientFd, err.c_str(), err.length(), 0);
//                     user->passMess = 0;
//                     continue;
//                 }
//                 continue;
//             }
//         }

//         // Commandes nécessitant un paramètre
//         if ((parsedMessage.command == "NICK" || parsedMessage.command == "USER" ||
//              parsedMessage.command == "JOIN" || parsedMessage.command == "PART") &&
//              parsedMessage.params.empty())
//         {
//             std::string errorMsg = ERR_NEEDMOREPARAMS(parsedMessage.command);
//             send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);
//             continue;
//         }

//         // Traitement des commandes
//         if (parsedMessage.command == "NICK")
//         {
//             ircManager.nickCommand(clientFd, parsedMessage.params);
//         }
//         else if (parsedMessage.command == "USER")
//         {
//             ircManager.userCommand(clientFd, parsedMessage.params);
//         }
//         else if (parsedMessage.command == "JOIN")
//         {
//             ircManager.joinCommand(clientFd, parsedMessage.params);
//         }
//         else if (parsedMessage.command == "PART")
//         {
//             std::string channelName;
//             std::istringstream iss(parsedMessage.params);
//             iss >> channelName;
//             std::string reason = parsedMessage.suffix;
//             ircManager.partCommand(clientFd, channelName, reason);
//         }
//         else if (parsedMessage.command == "PRIVMSG")
//         {
//             std::string channel, message;
//             if (parsedMessage.preparePRIVMSG(parsedMessage.params, parsedMessage.suffix, channel, message))
//             {
//                 ircManager.privmsgCommand(clientFd, channel, message);
//             }
//             else
//             {
//                 std::string errorMsg = ERR_NEEDMOREPARAMS(parsedMessage.command);
//                 send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);
//             }
//         }
//         else if (parsedMessage.command == "MODE")
//         {
//             std::string channelName, modeStr;
//             std::vector<std::string> modeParams;
//             if (parsedMessage.prepareMODE(parsedMessage.params, channelName, modeStr, modeParams))
//             {
//                 ircManager.modeCommand(clientFd, channelName, modeStr, modeParams);
//             }
//             else
//             {
//                 std::string errorMsg = ERR_NEEDMOREPARAMS(parsedMessage.command);
//                 send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);
//             }
//         }
//         else if (parsedMessage.command == "KICK")
//         {
//             std::string channel, target, reason;
//             if (parsedMessage.prepareKICK(parsedMessage.params, parsedMessage.suffix, channel, target, reason))
//             {
//                 ircManager.kickCommand(clientFd, channel, target, reason);
//             }
//             else
//             {
//                 std::string err = ERR_NEEDMOREPARAMS("KICK");
//                 send(clientFd, err.c_str(), err.length(), 0);
//             }
//         }
//         else if (parsedMessage.command == "INVITE")
//         {
//             std::string channel, target;
//             if (parsedMessage.prepareINVITE(parsedMessage.params, channel, target))
//             {
//                 ircManager.inviteCommand(clientFd, channel, target);
//             }
//             else
//             {
//                 std::string err = ERR_NEEDMOREPARAMS("INVITE");
//                 send(clientFd, err.c_str(), err.length(), 0);
//             }
//         }
//         else if (parsedMessage.command == "TOPIC")
//         {
//             std::string channelName;
//             std::istringstream iss(parsedMessage.params);
//             if (!(iss >> channelName))
//             {
//                 std::string err = ERR_NEEDMOREPARAMS("TOPIC");
//                 send(clientFd, err.c_str(), err.length(), 0);
//                 continue;
//             }
//             ircManager.topicCommand(clientFd, channelName, parsedMessage.suffix);
//         }
//         else if (parsedMessage.command != "WHO")
//         {
//             std::string errorMsg = ERR_UNKNOWNCOMMAND(parsedMessage.command);
//             send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);
//         }
//     }
// }


// // Gère les nouvelles connexions clients
// void Server::handleNewConnection()
// {
//     // Accepter la nouvelle connexion
//     struct sockaddr_in clientAddr;
//     memset(&clientAddr, 0, sizeof(clientAddr));
//     socklen_t clientAddrLen = sizeof(clientAddr);
//     int clientFd = accept(_serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
//     if (clientFd < 0)
//     {
//         std::cerr << "❌ Erreur : échec de l'acceptation de la connexion" << std::endl;
//         return;
//     }
//     if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)//rendre socket non bloquant
//     {
//         std::cerr << "Erreur lors du fcntl (F_SETFL)" << std::endl;
//         close(clientFd);
//         return;
//     }
//     // Ajout du socket client à la liste de poll
//     struct pollfd clientPollFd;
//     clientPollFd.fd = clientFd;
//     clientPollFd.events = POLLIN;
//     clientPollFd.revents = 0;
//     _pollFds.push_back(clientPollFd);
//     std::cout << "✅ Nouveau client connecté (FD : " << clientFd << ")" << std::endl;
//     // Ajouter un nouvel utilisateur dans IRCManager
//     ircManager.newUser(clientFd);
// }






// //------------------------------------------------------------------------------------------

// extern volatile sig_atomic_t running;
// // Boucle principale du serveur
// void Server::run()
// {
// 	while (running)
// 	{
// 		int pollCount = poll(_pollFds.data(), _pollFds.size(), -1);
// 		if (pollCount < 0)
// 		{
// 			if (errno == EINTR)
// 				break; // Quitte la boucle proprement
// 			else
// 			{
// 				std::cerr << "⚠️❌ Erreur : poll() a échoué" << std::endl;
// 				exit(EXIT_FAILURE);
// 			}
// 		}
// 		for (size_t i = 0; i < _pollFds.size(); i++)
// 		{
// 			if (_pollFds[i].revents & POLLIN)
// 			{
// 				if (_pollFds[i].fd == _serverSocket)
// 					handleNewConnection();
// 				else
// 					handleClientMessage(_pollFds[i].fd);
// 			}
// 		}
// 	}
// }
// //----------------------------------------------------------------------------------------------------------------------------------------------------------
// //----------------------------------------------------------------------------------------------------------------------------------------------------------




// //--------------------------------------------------------------------DECONEXION D UN CLIENT-----------------------------------------------------------------
// //-----------------------------------------------------------------------------------------------------------------------------------------------------------
// // Supprime un client qui s'est déconnecté
// void Server::removeClient(int clientFd)
// {
//     close(clientFd);
//     // Supprimer le client de _pollFds
//     for (size_t i = 0; i < _pollFds.size(); i++)
//     {
//         if (_pollFds[i].fd == clientFd)
//         {
//             _pollFds.erase(_pollFds.begin() + i);
//             break;
//         }
//     }
//     // Faire quitter tous les canaux à l'utilisateur
//     User *user = ircManager.getUser(clientFd);
//     if (user)
//     {
//         std::map<std::string, Channel*>& channels = ircManager.getChannels();
//         std::vector<std::string> joinedChannels;

//         // On copie (sinon leaks puisque part efface le channel) les noms des channels où est le user
//         for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
//         {
//             if (it->second->isMember(user))
//                 joinedChannels.push_back(it->first);
//         }

//         // Ensuite on appelle partCommand sans toucher aux itérateurs de la map
//         for (size_t i = 0; i < joinedChannels.size(); ++i)
//         {
//             ircManager.partCommand(clientFd, joinedChannels[i], "");
//         }
//     }

//     // Supprimer l'utilisateur de IRCManager
//     ircManager.removeUser(clientFd);
// }
// //-----------------------------------------------------------------------------------------------------------------------------------------------------------
// //-----------------------------------------------------------------------------------------------------------------------------------------------------------






















/*
-----------------------------------------------------------ETAPES-----------------------------------
1️⃣ Initialisation du serveur (Server::Server())

Création du socket

Configuration de l'adresse (sockaddr_in)

bind() pour attacher le socket à un port

listen() pour accepter les connexions

Ajout du serveur à poll()

2️⃣ Acceptation des connexions (handleNewConnection())

Vérification de poll() pour voir si un client veut se connecter

accept() pour accepter la connexion

Passage du socket en mode non bloquant (fcntl())

Ajout du client à poll()

3️⃣ Réception des messages (handleClientMessage())

recv() pour lire les données

Si recv() <= 0, on considère le client déconnecté (removeClient())

Sinon, on affiche le message et on fait un "Echo Server" en renvoyant le message

4️⃣ Suppression des clients (removeClient())

Fermeture du socket

Suppression du client de poll()

5️⃣ Boucle principale (run())

poll() pour surveiller les événements

Gestion des nouvelles connexions

Gestion des messages des clients

---------------------------------------------------------------RESUME-----------------------------------
Le serveur crée un socket et écoute sur le port donné.

Il ajoute ce socket à poll() pour surveiller les nouvelles connexions.

Quand une connexion arrive :

accept() l'accepte.

fcntl() rend le socket non bloquant.

poll() ajoute le client à la liste.

Quand un client envoie un message :

recv() le lit.

Si le message est vide (pas un mes vide "" mais si ca retourne 0), le client est déconnecté (removeClient()).

Sinon, le message est affiché et renvoyé (serveur Echo pour l’instant).

La boucle poll() gère tout sans bloquer :

Si une nouvelle connexion arrive → handleNewConnection().

Si un client envoie un message → handleClientMessage().

Si un client se déconnecte → removeClient().
------------------------------------------------------------------------------------------------------
*/





//DETAIL CONSTRUCTION DU SERVEUR (via constructeur)
//-------------------------------------------------------------------------------------------------------------------------
/*
(1)
3 arguments pour socket
socket(domain, type, protocol);

domain	               AF_INET	                  Indique que c'est un socket pour IPv4 (Internet).
type	             SOCK_STREAM	              Indique que c'est un socket TCP (connexion fiable, flux continu).
protocol	               0	                  Laisse le système choisir le protocole par défaut (ici TCP).
---------------------------------------------------------------------------------------------------------------------------
(2)
memset(&serverAddr, 0, sizeof(serverAddr));
Cette ligne remplit serverAddr avec des zéros (efface tout).
pour éviter des valeurs non initialisées.


serverAddr.sin_family = AF_INET;
Définit le type d’adresse utilisé par le socket : IPv4 (AF_INET).
Si on voulait IPv6, on utiliserait AF_INET6.

serverAddr.sin_addr.s_addr = INADDR_ANY;
Assigne l’adresse IP du serveur.
INADDR_ANY signifie "écouter sur toutes les interfaces réseau disponibles".
Cela permet au serveur d'accepter des connexions sur toutes les adresses IP de la machine (ex : 127.0.0.1, 192.168.x.x, 10.x.x.x...).


serverAddr.sin_port = htons(_port);
Définit le port sur lequel le serveur va écouter.
_port est la variable qui stocke le port passé en argument.
htons() (Host TO Network Short) convertit _port en format big-endian.
Les réseaux utilisent big-endian, alors que certaines machines (Intel x86) utilisent little-endian.
htons() s'assure que le port est bien compris sur le réseau.
--------------------------------------------------------------------------------------------------------------------------------
(3)
La fonction bind() prend trois arguments :

bind(socket_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

socket_fd	 :Le descripteur du socket (ici _serverSocket).
address	     :Un pointeur vers une structure sockaddr contenant l'IP et le port.
size	     :La taille de la structure sockaddr_in passée en paramètre.


🔹 Que fait bind() ?
Associe le socket à une adresse IP et un port (ceux définis dans serverAddr).
C'est une étape obligatoire avant de mettre le serveur en écoute avec listen().

🔹 Pourquoi bind() peut échouer ?
Si bind() renvoie -1, c’est qu’il y a une erreur, comme :
Le port est déjà utilisé ❌ (un autre serveur tourne sur ce port).
Manque de permissions ❌ (les ports <1024 nécessitent des privilèges root).
Adresse non valide ❌ (serverAddr mal configuré).
---------------------------------------------------------------------------------------------------------------------------------
(4)
2 arguments pour listen
listen(socket_fd, backlog);

socket_fd :	Le descripteur de fichier du socket (_serverSocket).
backlog	 : Le nombre maximum de connexions en attente dans la file d'attente avant que les connexions soient refusées. Ici, c’est 10.

🔹 Ce que fait listen()
Mise en écoute du socket.
Le serveur attend maintenant que des clients se connectent à ce socket.
Le deuxième argument, backlog, spécifie la taille de la file d'attente pour les connexions entrantes.
Si un client essaie de se connecter alors que la file d'attente est pleine, la connexion sera refusée jusqu'à ce qu'il y ait de la place dans la file.
Ici, 10 indique qu'on peut avoir jusqu’à 10 connexions en attente
---------------------------------------------------------------------------------------------------------------------------------
(5)
Ce code permet d'ajouter le socket du serveur à une liste de descripteurs de fichiers surveillés par poll().

Ajouter le socket du serveur à poll()

struct pollfd serverPollFd :

serverPollFd.fd = _serverSocket;
serverPollFd.events = POLLIN;
_pollFds.push_back(serverPollFd);

struct pollfd serverPollFd;
Déclare une structure pollfd qui représente un descripteur de fichier surveillé par poll().

Cette structure contient trois champs :
struct pollfd
{
    int   fd;      // Descripteur de fichier (socket ici)
    short events;  // Événements surveillés (lecture, écriture, etc.)
    short revents; // Événements détectés (rempli après poll())
};

serverPollFd.fd = _serverSocket;
On assigne à fd le descripteur du socket du serveur (_serverSocket).
Cela signifie qu'on va surveiller les événements sur ce socket.

serverPollFd.events = POLLIN;
On indique qu'on veut surveiller l'événement POLLIN, c'est-à-dire si le socket est prêt à lire.
Concrètement, cela permet de détecter une nouvelle connexion entrante sur le socket du serveur.
POLLIN est utilisé pour savoir si des données sont disponibles en lecture ou si une connexion entrante est prête à être acceptée.

_pollFds.push_back(serverPollFd);
On ajoute serverPollFd à un vecteur _pollFds, qui stocke tous les descripteurs surveillés.
_pollFds est sûrement un std::vector<struct pollfd> qui stocke tous les sockets surveillés (le serveur + les clients).
poll() sera ensuite utilisé pour vérifier l'état de ces descripteurs.

Pourquoi utiliser poll() ici ?
Contrairement à select(), poll() permet de gérer plusieurs connexions simultanément sans limite stricte sur le nombre de sockets.
Il permet d'attendre de nouveaux clients sans bloquer le programme.
Quand poll() détecte que _serverSocket est prêt en lecture, ça signifie qu'un client tente de se connecter → on pourra alors l’accepter avec accept().

Résumé
Ajoute le socket du serveur à poll() pour détecter les nouvelles connexions.
Utilise POLLIN pour surveiller les événements de lecture.
Stocke le socket dans _pollFds, qui sera passé à poll() pour vérifier l'activité réseau.
*/
