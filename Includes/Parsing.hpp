/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yonieva <yonieva@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:16:35 by yonieva           #+#    #+#             */
/*   Updated: 2025/04/02 16:32:06 by yonieva          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_HPP
#define PARSING_HPP

#include <iostream>
#include <vector>
#include <sstream>

class Parsing 
{
public:
    Parsing();
    ~Parsing();

    std::vector<std::string> parseCommand(const std::string &message);
};

#endif



