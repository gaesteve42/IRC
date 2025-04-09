/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:28:56 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/09 13:45:14 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Include.hpp"

volatile sig_atomic_t running = 1;
/*
pour faire simple : sig atomic t c'est un levier de frein à main, peu importe à quel point tu tires dessus il cassera pas
et volatile c'est comme si on demande au compilateur de toujours vérifier l'état du levier

l'explication de tout ça par tonton :

- - - - Pourquoi sig_atomic_t ? C’est un type spécial prévu pour être utilisé dans les signaux.
Il garantit que la variable sera toujours mise à jour de manière cohérente et sûre,
même dans le cas improbable où plusieurs signaux arrivent presque simultanément.

- - - - Pourquoi volatile ? Pour demander explicitement au compilateur de ne jamais optimiser la lecture ou l'écriture de cette variable.
La valeur sera toujours vérifiée directement, jamais "mise en cache". Ainsi, le programme peut réagir immédiatement et correctement au signal.

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
