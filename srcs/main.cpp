/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:28:56 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/03 15:17:16 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*----------------------------------------TEST MAIN (GAUTHIER)----------------------------------------------------------

// POUR TESTER MODE ET LES COMMANDES COMME NICK,USER, JOIN
// #include <iostream>
// #include "../Includes/IRCManager.hpp"

// int main()
// {
// 	IRCManager ircManager;
// 	// Simulation de deux utilisateurs se connectant au serveur
// 	int fdUser1 = 1;
// 	int fdUser2 = 2;
// 	ircManager.newUser(fdUser1);
// 	ircManager.newUser(fdUser2);
// 	// Authentification complète (NICK + USER requis)
// 	ircManager.nickCommand(fdUser1, "Gauthier");
// 	ircManager.userCommand(fdUser1, "Yoan");
// 	ircManager.nickCommand(fdUser2, "Kevin");
// 	ircManager.userCommand(fdUser2, "Nael");
// 	// Alice crée et rejoint un canal
// 	ircManager.joinCommand(fdUser1, "#general");
// 	// Alice devient opératrice du canal (simulé directement)
// 	ircManager.getUser(fdUser1)->setOperator(true);
// 	// Alice met le canal en invite-only et limite les utilisateurs à 2
// 	ircManager.modeCommand(fdUser1, "#general", "+i");
// 	ircManager.modeCommand(fdUser1, "#general", "+l", "2");
// 	// Alice invite Bob
// 	ircManager.modeCommand(fdUser1, "#general", "+o", "Gauthier"); // Bob devient opérateur
// 	// Bob essaie de rejoindre le canal (invite-only)
// 	ircManager.joinCommand(fdUser2, "#general"); // Normalement devrait fonctionner (mais invite à gérer)
// 	// Alice définit un mot de passe pour le canal
// 	ircManager.modeCommand(fdUser1, "#general", "+k", "123");
// 	// Afficher état final
// 	std::cout << "\nÉtat final :" << std::endl;
// 	std::cout << "Canal #general en invite-only, limite à 2, clé '123'." << std::endl;
// 	// Déconnexion des utilisateurs
// 	ircManager.removeUser(fdUser1);
// 	ircManager.removeUser(fdUser2);
// 	return 0;
// }

/*----------------------------------------TEST MAIN (YOAN)----------------------------------------------------------

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
