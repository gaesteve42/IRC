/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCManager.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 12:04:06 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/02 14:18:06 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCMANAGER_HPP
#define IRCMANAGER_HPP

#include "User.hpp"
#include "Channel.hpp"
#include <map>
#include <sstream>
#include <iostream>

class IRCManager
{
private:
	std::map<int, User*> users;
	std::map<std::string, Channel*> channels;

public:
	IRCManager();
	~IRCManager();

	// Gestion des utilisateurs
	void newUser(int fd);
	void removeUser(int fd);
	User* getUser(int fd);

	// Commandes IRC (déjà parsées par ton partenaire)
	void joinCommand(int fd, const std::string &channelName);
	void partCommand(int fd, const std::string &channelName);
	void privmsgCommand(int fd, const std::string &channelName, const std::string &message);
};

#endif

/*
📖 **Explication de la classe IRCManager :**

Cette classe agit comme un point central (une sorte de gestionnaire général) pour relier les connexions réseau gérées par le serveur (sockets) avec la logique IRC spécifique (Users et canaux).

🔗 **Pourquoi utiliser une map (std::map) ?**
- Une `map` est un conteneur associatif qui stocke des données sous forme de paires clé-valeur.
- Elle permet de retrouver rapidement un élément à partir d’une clé unique.

Dans notre cas précis, nous utilisons deux maps :

1. 📌 **Map des Users (`Users`) :**
    - Clé (`int`) : c’est le **descripteur de fichier (fd)** du socket connecté (unique pour chaque utilisateur connecté).
    - Valeur (`User*`) : c’est un pointeur vers l’objet `User` qui représente un utilisateur IRC (son pseudo, username, etc.).

    ➡️ **Pourquoi faire ça ?**
    - Ça permet très facilement de relier chaque socket à un utilisateur précis.
    - Quand un message arrive d’un socket, on récupère directement le User associé sans chercher longuement dans une liste.

2. 📌 **Map des canaux (`channels`) :**
    - Clé (`std::string`) : c’est le **nom du canal** (par exemple "#general").
    - Valeur (`Channel*`) : pointeur vers l’objet qui représente ce canal IRC spécifique.

    ➡️ **Pourquoi faire ça ?**
    - Facilite énormément la gestion des canaux (accès instantané, création simple).
    - Lorsqu'un User demande de rejoindre un canal, il suffit de vérifier si le canal existe déjà ou s'il faut le créer directement dans la map.

⚙️ **Pourquoi cette implémentation précise ?**

Cette implémentation est simple, efficace, rapide et particulièrement adaptée aux contraintes du projet IRC (gérer rapidement des utilisateurs et des salons par noms ou identifiants uniques).

Elle permet une complexité très faible lors des recherches (O(log n)), ce qui est très efficace même avec beaucoup d’utilisateurs ou de canaux.

📍 **Que fait précisément cette classe IRCManager ?**

- Lorsqu'un User se connecte au serveur (accepté via un socket), `IRCManager` crée un nouveau `User` et l'associe immédiatement à son socket.
- À chaque fois qu'un message IRC est reçu par le serveur, `IRCManager` analyse ce message (parsing) pour déterminer ce que l'utilisateur veut faire (JOIN, PART, PRIVMSG, etc.).
- Elle modifie ensuite directement les objets `User` et `Channel` pour refléter ces actions (ajoute un User dans un canal, retire un User d'un canal, diffuse des messages, etc.).

En résumé, cette classe centralise, simplifie et organise la logique complète du serveur IRC, assurant ainsi une intégration propre entre la partie réseau (sockets) et la partie gestion IRC (User, Channel).
*/
