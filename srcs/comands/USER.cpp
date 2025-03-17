/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   USER.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:39:17 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/17 20:37:55 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include <iostream>
#include <cstring>
#include <sstream>
#include <sys/socket.h>

void CommandHandler::handleUserCommand(Client &client, const std::string &message) {
    std::istringstream iss(message);
    std::string username, hostname, servername, realname;
    
    iss >> username >> hostname >> servername; // Leer los primeros 4 parámetros

    std::getline(iss >> std::ws, realname);

    std::cout << "[DEBUG] USER recibido - Username: " << username 
              << ", Hostname: " << hostname 
              << ", Servername: " << servername 
              << ", Realname: " << realname << std::endl;

    // Validaciones
    if (username.empty() || realname.empty()) {
        const char* error_msg = "ERROR: Formato incorrecto de USER.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    // Asignar valores al cliente
    client.setUsername(username);
    client.setRealname(realname);
    client.authenticate();

    if (client.isAuthenticated()) {
        const char* success_msg = "Autenticación completa.\n";
        send(client.getFd(), success_msg, strlen(success_msg), 0);
    } else {
        const char* warning_msg = "Warning: Falta el NICK para completar la autenticación.\n";
        send(client.getFd(), warning_msg, strlen(warning_msg), 0);
    }
}
