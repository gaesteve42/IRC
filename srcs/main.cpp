/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:28:56 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/09 21:03:07 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Include.hpp"

volatile sig_atomic_t running = 1;
/*
VOLATILE
à chaque boucle du serveur, on vérifie si on a reçu un signal pour stop.

SIG_ATOMIC_T
sécurité quand la variable change par un signal.
*/

void signalHandler(int signalNum)
{
	std::cout << "\nSignal reçu (" << signalNum << "), fermeture du serveur !\n";
	running = 0;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	int port = std::atoi(argv[1]);
	if (port <= 0 || port > 65535)
	{
		std::cerr << "❌ Port invalide. Utilisez un port entre 1 et 65535." << std::endl;
		return 1;
	}
	std::string password = argv[2];
	std::signal(SIGINT, signalHandler);
	std::signal(SIGQUIT, signalHandler);
	try
	{
		Server ircServer(port, password);
		while (running)
			ircServer.run(); // Lance la boucle principale
	}
	catch (const std::exception &e)
	{
		std::cerr << "Erreur : " << e.what() << std::endl;
		return 1;
	}
	//ici on fermera proprement le serveur
	return 0;
}
