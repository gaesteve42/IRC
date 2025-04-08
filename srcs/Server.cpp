/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yonieva <yonieva@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 15:07:45 by yonieva           #+#    #+#             */
/*   Updated: 2025/04/08 17:13:45 by yonieva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Include.hpp"

//--------------------------------------------------------------------CONSTUCTION DU SERVEUR-----------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructeur du serveur
Server::Server(int port, std::string pass) : _port(port), _pass(pass)
{
	// Création du socket du serveur
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket < 0)
	{
		std::cerr << "❌Erreur❌ : Impossible de créer le socket" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Permet de réutiliser rapidement l'adresse et le port après un arrêt
	int opt = 1;
	setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// Configuration de l'adresse du serveur
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(_port);

	// Attacher le socket à l'adresse et au port
	if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "❌Erreur❌ : Bind échoué" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Écoute des connexions entrantes
	if (listen(_serverSocket, 10) < 0)
	{
		std::cerr << "❌Erreur❌ : Listen échoué" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Ajouter le socket du serveur à poll()
	struct pollfd serverPollFd;
	serverPollFd.fd = _serverSocket;
	serverPollFd.events = POLLIN;
	_pollFds.push_back(serverPollFd);

	std::cout << "✅ Serveur lancé sur le port " << _port << std::endl;
}

//----------------------------------------------------------------------------------------

// Destructeur : ferme le socket principal et tous les clients
Server::~Server()
{
	close(_serverSocket);
	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		close(_pollFds[i].fd);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------





//--------------------------------------------------------------------SERVEUR EN ROUTE--------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------
// Gère les nouvelles connexions clients
void Server::handleClientMessage(int clientFd)
{
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));

    int bytesReceived = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived <= 0)
    {
        std::cout << "❌ Client déconnecté (FD : " << clientFd << ")" << std::endl;
        removeClient(clientFd);
        ircManager.removeUser(clientFd);
        return;
    }

    std::string message(buffer);
    Parsing parsedMessage;
    parsedMessage.parseCommand(message);  // Découpe le message en prefix, command, params, suffix

    User *user = ircManager.getUser(clientFd);

    if (!user)
        return;

    // Check si user a donné le bon mot de passe
    if (!user->hasProvidedPassword())
    {
        if (parsedMessage.command == "PASS")
        {
            if (parsedMessage.params == _pass)
            {
                user->setHasProvidedPassword(true);
                std::cout << "✅ Mot de passe accepté pour FD " << clientFd << std::endl;
                return;
            }
            else
            {
                std::cout << "❌ Mauvais mot de passe pour FD " << clientFd << std::endl;
                std::string err = "ERROR :Mot de passe incorrect\r\n";
                send(clientFd, err.c_str(), err.length(), 0);
                removeClient(clientFd);
                ircManager.removeUser(clientFd);
                return;
            }
        }
        else
        {
            std::string err = "ERROR :Mot de passe requis\r\n";
            send(clientFd, err.c_str(), err.length(), 0);
            return;
        }
    }

    // Vérification des commandes avec un paramètre
    if ((parsedMessage.command == "NICK" || parsedMessage.command == "USER" || parsedMessage.command == "JOIN" || 
         parsedMessage.command == "PART") && parsedMessage.params.empty())
    {
        std::string errorMsg = ERR_NEEDMOREPARAMS(parsedMessage.command);
        send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);
        return;
    }

    // Traitement des commandes----------------------------------------------------------
    if (parsedMessage.command == "NICK")
    {
        ircManager.nickCommand(clientFd, parsedMessage.params);
    }
    else if (parsedMessage.command == "USER")
    {
        ircManager.userCommand(clientFd, parsedMessage.params);
    }
    else if (parsedMessage.command == "JOIN")
    {
        ircManager.joinCommand(clientFd, parsedMessage.params);
    }
    else if (parsedMessage.command == "PART")
    {
        ircManager.partCommand(clientFd, parsedMessage.params);
    }
    else if (parsedMessage.command == "PRIVMSG")
    {
        std::string channel, message;
        if (parsedMessage.preparePRIVMSG(parsedMessage.params, channel, message))
        {
            ircManager.privmsgCommand(clientFd, channel, message);
        }
        else
        {
            std::string errorMsg = ERR_NEEDMOREPARAMS(parsedMessage.command);
            send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);
        }
    }
    else if (parsedMessage.command == "MODE")
    {
        std::string channelName, mode, param;
        if (parsedMessage.prepareMODE(parsedMessage.params, channelName, mode, param))
        {
            ircManager.modeCommand(clientFd, channelName, mode, param);
        }
        else
        {
            std::string errorMsg = ERR_NEEDMOREPARAMS(parsedMessage.command);
            send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);
        }
    }
    else if (parsedMessage.command == "KICK")
    {
        std::string channel, target, reason;
        if (parsedMessage.prepareKICK(parsedMessage.params, channel, target, reason))
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
        std::string channel, topic;
        if (parsedMessage.prepareTOPIC(parsedMessage.params, channel, topic))
            ircManager.topicCommand(clientFd, channel, topic);
        else
        {
            std::string err = ERR_NEEDMOREPARAMS("TOPIC");
            send(clientFd, err.c_str(), err.length(), 0);
        }
    }
    else
    {
        std::string errorMsg = ERR_UNKNOWNCOMMAND(parsedMessage.command);
        send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);
    }
}



//------------------------------------------------------------------------------------------

// Boucle principale du serveur
void Server::run()
{
	while (true)
	{
		int pollCount = poll(_pollFds.data(), _pollFds.size(), -1);
		if (pollCount < 0)
		{
			std::cerr << "⚠️❌ Erreur : poll() a échoué" << std::endl;
			exit(EXIT_FAILURE);
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
//----------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------




//--------------------------------------------------------------------DECONEXION D UN CLIENT-----------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
// Supprime un client qui s'est déconnecté
void Server::removeClient(int clientFd)
{
    close(clientFd);

    //Supprimer le client de _pollFds
    for (size_t i = 0; i < _pollFds.size(); i++)
    {
        if (_pollFds[i].fd == clientFd)
        {
            _pollFds.erase(_pollFds.begin() + i);
            break;
        }
    }


    //Faire quitter tous les canaux à l'utilisateur
    User *user = ircManager.getUser(clientFd);
    if (user)
    {
        for (std::map<std::string, Channel*>::iterator it = ircManager.getChannels().begin(); it != ircManager.getChannels().end(); ++it)
        {
            if (it->second->isMember(user))
            {
                ircManager.partCommand(clientFd, it->first);
            }
        }
    }

    //Supprimer le client de IRCManager
    ircManager.removeUser(clientFd);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------






















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
