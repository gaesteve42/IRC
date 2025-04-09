/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaesteve <gaesteve@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:29:00 by gaesteve          #+#    #+#             */
/*   Updated: 2025/04/09 17:43:38 by gaesteve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include "../Includes/Include.hpp"

class User
{
private:
	int fd;
	std::string nickname;
	std::string username;
	std::string hostname;
	bool isOperator;
	bool _hasProvidedPassword;
	bool authenticated;
public:
	// Constructeurs
	User();
	User(int fd);
	User(int fd, const std::string &nick, const std::string &user, const std::string &host);

	std::string getNickname() const;
	std::string getUsername() const;
	std::string getHostname() const;
	bool getIsOperator() const;
	bool hasProvidedPassword() const;
	void setHasProvidedPassword(bool val);
	bool isAuthenticated() const;
	int getFd() const;

	void setNickname(const std::string &nick);
	void setUsername(const std::string &user);
	void setHostname(const std::string &host);
	void setOperator(bool op);
	void setAuthenticated(bool auth);
};

#endif

