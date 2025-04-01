/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yonieva <yonieva@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 15:05:58 by yonieva           #+#    #+#             */
/*   Updated: 2025/04/01 16:49:13 by yonieva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>  // Pour memset
#include <cstdlib>

class Server {
private:
    int _serverSocket;                  // Socket principal du serveur
    int _port;                           // Port sur lequel on écoute
    std::vector<struct pollfd> _pollFds; // Liste des sockets surveillés avec poll()

    void handleNewConnection();          // Gère les nouvelles connexions
    void handleClientMessage(int clientFd); // Gère les messages reçus des clients
    void removeClient(int clientFd);     // Supprime un client qui s'est déconnecté

public:
    Server(int port);
    ~Server();
    void run();  // Boucle principale du serveur
};

#endif



/*
Création du socket et écoute sur le port donné en argument

Gestion des connexions entrantes (acceptation de nouveaux clients)

Lecture des messages des clients (affichage des messages reçus)

Utilisation de poll() pour gérer plusieurs clients sans blocage
*/

