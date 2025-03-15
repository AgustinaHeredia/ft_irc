/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   USER.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:39:17 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/15 17:26:55 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>

void CommandHandler::handleUserCommand(Client &client, const std::string &user) {
    if (user.empty()) {
        const char* error_msg = "ERROR: Username inválido.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

	client.setUsername(user);
    client.authenticate();

    if (client.isAuthenticated()) {
        const char* success_msg = "Autenticación completa.\n";
        send(client.getFd(), success_msg, strlen(success_msg), 0);
    } else {
        const char* warning_msg = "Warning: Falta el NICK para completar la autenticación.\n";
        send(client.getFd(), warning_msg, strlen(warning_msg), 0);
    }
}
