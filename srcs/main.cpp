/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:28:56 by gaesteve          #+#    #+#             */
/*   Updated: 2025/03/31 17:35:47 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// main.cpp
#include <iostream>
#include "../Includes/Client.hpp"
#include "../Includes/Channel.hpp"


//            MAIN TEST DES CLASSES CHANNEL ET CLIENT, CHECK DE L IMPLEMENTATION ( tu peux suppr si tu veux, c'est osef je voulais juste voir si tout fonctionnait)
// int main()
// {
// 	// Création des clients
// 	Client client1("Guluguts", "Gauthier", "192.168.1.2");
// 	Client client2("Fatabot", "Yoan", "192.168.1.3");
// 	// Affichage des informations client
// 	std::cout << "Client 1: " << client1.getNickname() << " (" << client1.getUsername() << ") - IP: " << client1.getHostname() << std::endl;
// 	std::cout << "Client 2: " << client2.getNickname() << " (" << client2.getUsername() << ") - IP: " << client2.getHostname() << std::endl;
// 	// Création d'un canal
// 	Channel channel("#LobbyRanked");
// 	// Configuration du canal
// 	channel.setTopic("Bienvenue dans la partie ranked !");
// 	channel.setInviteOnly(true);
// 	channel.setKey("secret123");
// 	channel.setUserLimit(5);
// 	// Ajout des clients au canal
// 	if (channel.addMember(&client1))
// 		std::cout << client1.getNickname() << " a rejoint " << channel.getChannelName() << std::endl;
// 	else
// 		std::cout << client1.getNickname() << " n'a pas pu rejoindre " << channel.getChannelName() << std::endl;

// 	if (channel.addMember(&client2))
// 		std::cout << client2.getNickname() << " a rejoint " << channel.getChannelName() << std::endl;
// 	else
// 		std::cout << client2.getNickname() << " n'a pas pu rejoindre " << channel.getChannelName() << std::endl;
// 	// Affichage des infos du canal
// 	std::cout << "Canal: " << channel.getChannelName() << " | Sujet: " << channel.getTopic() << " | Membres: " << channel.getMembers().size() << std::endl;
// 	// Affichage des membres du canal
// 	for (size_t i = 0; i < channel.getMembers().size(); ++i)
// 	{
// 		Client *member = channel.getMembers()[i];
// 		std::cout << "- " << member->getNickname() << std::endl;
// 	}
// 	// Retirer un membre
// 	channel.removeMember(&client1);
// 	std::cout << client1.getNickname() << " a quitté " << channel.getChannelName() << std::endl;
// 	// Vérification finale des membres
// 	std::cout << "Membres restants dans " << channel.getChannelName() << " : " << channel.getMembers().size() << std::endl;
// 	for (size_t i = 0; i < channel.getMembers().size(); ++i)
// 	{
// 		Client *member = channel.getMembers()[i];
// 		std::cout << "- " << member->getNickname() << std::endl;
// 	}
// 	return 0;
// }
