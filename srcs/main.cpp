/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:28:56 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/03 16:36:37 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Include.hpp"

//----------------------------------------TEST MAIN (GAUTHIER)----------------------------------------------------------

int main()
{
	IRCManager irc;

	// Simuler deux utilisateurs
	int fd1 = 1;
	int fd2 = 2;
	irc.newUser(fd1);
	irc.newUser(fd2);
	irc.nickCommand(fd1, "Alice");
	irc.userCommand(fd1, "alice42");
	irc.nickCommand(fd2, "Bob");
	irc.userCommand(fd2, "bob42");
	// Alice crée un canal
	irc.joinCommand(fd1, "#testchan");
	irc.getUser(fd1)->setOperator(true); // elle devient opérateur
	// Alice met le canal en invite-only
	irc.modeCommand(fd1, "#testchan", "+i");
	// Bob tente de rejoindre (doit échouer)
	irc.joinCommand(fd2, "#testchan");
	// Alice invite Bob
	irc.inviteCommand(fd1, "#testchan", "Bob");
	// Bob rejoint (doit réussir)
	irc.joinCommand(fd2, "#testchan");
	// Alice définit un sujet
	irc.topicCommand(fd1, "#testchan", "Bienvenue sur le chan privé !");
	// Bob consulte le sujet
	irc.topicCommand(fd2, "#testchan", "");
	// Alice kick Bob
	irc.kickCommand(fd1, "#testchan", "Bob", "Tu spams trop !");
	// Suppression des users
	irc.removeUser(fd1);
	irc.removeUser(fd2);
	return 0;
}

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
