/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:28:56 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/02 14:17:43 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "../Includes/User.hpp"
// #include "../Includes/Channel.hpp"
// #include "../Includes/IRCManager.hpp"

/*----------------------------------------TEST MAIN (Gauthier)----------------------------------------------------------
// int main()
// {
// 	IRCManager ircManager;

// 	// Simulation de deux utilisateurs se connectant au serveur
// 	int fdUser1 = 1; // faux descripteur de fichier (fd simulé)
// 	int fdUser2 = 2;

// 	ircManager.newUser(fdUser1);
// 	ircManager.newUser(fdUser2);

// 	// Attribution des nicknames aux utilisateurs pour les tests
// 	ircManager.getUser(fdUser1)->setNickname("Alice");
// 	ircManager.getUser(fdUser2)->setNickname("Bob");

// 	// Test de la commande JOIN pour rejoindre un canal
// 	ircManager.handleMessage(fdUser1, "JOIN #channel1");
// 	ircManager.handleMessage(fdUser2, "JOIN #channel1");

// 	// Test de la commande PRIVMSG pour envoyer un message dans le canal
// 	ircManager.handleMessage(fdUser1, "PRIVMSG #channel1 Bonjour tout le monde !");

// 	// Test de la commande PART pour quitter un canal
// 	ircManager.handleMessage(fdUser2, "PART #channel1");

// 	// Vérification après la sortie d'un utilisateur
// 	ircManager.handleMessage(fdUser1, "PRIVMSG #channel1 Où est passé Bob ?");

// 	// Déconnexion des utilisateurs
// 	ircManager.removeUser(fdUser1);
// 	ircManager.removeUser(fdUser2);

// 	return 0;
// }
-----------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------TEST MAIN SERVER (YOAN)----------------------------------------------------------

#include "../Includes/Server.hpp"

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535)
    {
        std::cerr << "Erreur : Port invalide" << std::endl;
        return 1;
    }

    Server server(port);
    server.run();  // Démarre le serveur
}

dans le terminal :
yoan@OKAMA-GAMESPHERE:~/IRC/srcs$ g++ -Wall -Wextra -Werror -std=c++98 Server.cpp main.cpp -o ircserv
yoan@OKAMA-GAMESPHERE:~/IRC/srcs$ ./ircserv 6667

-----------------------------------------------------------------------------------------------------------------------*/
