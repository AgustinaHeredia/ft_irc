/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   USER.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:39:17 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/30 17:32:50 by agusheredia      ###   ########.fr       */
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
    
    iss >> username >> hostname >> servername; // Leer los primeros 3 parámetros

    std::getline(iss >> std::ws, realname); // Leer el realname

    std::cout << "[DEBUG] USER received - Username: " << username 
              << ", Hostname: " << hostname 
              << ", Servername: " << servername 
              << ", Realname: " << realname << std::endl;

    // Validaciones
    if (username.empty() || realname.empty()) {
        const char* error_msg = "ERROR: Incorrect USER format.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    // Verificar que ya se haya enviado un NICK
    if (client.getNickname().empty()) {
        const char* error_msg = "ERROR: NICK is missing before USER.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    // Asignar valores al cliente
    client.setUsername(username);
    client.setRealname(realname);
    client.authenticate();

    // Enviar mensaje de bienvenida si la autenticación es exitosa
    if (client.isAuthenticated()) {
        std::string welcome_msg = ":127.0.0.1 001 " + client.getNickname() + " :Welcome to the IRC server\r\n";
        send(client.getFd(), welcome_msg.c_str(), welcome_msg.size(), 0);
    } else {
        const char* warning_msg = "Warning: NICK is missing to complete authentication.\n";
        send(client.getFd(), warning_msg, strlen(warning_msg), 0);
    }
}
