/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 19:56:47 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/02 14:39:02 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include <sstream>

std::string intToString(int number) {
    std::ostringstream oss;
    oss << number;
    return oss.str();
}
