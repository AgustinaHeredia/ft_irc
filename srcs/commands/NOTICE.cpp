/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NOTICE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 18:00:52 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/04 13:10:07 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>

void CommandHandler::handleNotice(Server &srv, Client &client, const std::string &message) {
    std::cout << "[DEBUG] Received NOTICE: " << message << std::endl;

    // Verificar si el cliente está autenticado
    if (!client.isAuthenticated()) {
        const char* error_msg = "ERROR: Not authenticated.\r\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        std::cout << "[DEBUG] Unauthenticated client, rejecting NOTICE" << std::endl;
        return;
    }

    std::istringstream iss(message);
    std::string target, msg;

    // Extraer el destinatario
    iss >> target;

    // Verificar si el destinatario está vacío
    if (target.empty()) {
        const char* error_msg = "ERROR: Incorrect use of NOTICE in target.\r\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        std::cout << "[DEBUG] Error: Empty Target in NOTICE" << std::endl;
        return;
    }

    // Extraer el mensaje completo después del target
    if (iss.peek() == ':') {
        iss.get(); // Eliminar el carácter ':'
    }
    std::getline(iss, msg);

    // Verificar si el mensaje está vacío
    if (msg.empty()) {
        const char* error_msg = "ERROR: Incorrect use of NOTICE. Empty message.\r\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        std::cout << "[DEBUG] Error: Empty message in NOTICE" << std::endl;
        return;
    }

    std::cout << "[DEBUG] NOTICE of " << client.getNickname() << " from " << target << ": " << msg << std::endl;

    // Manejo de múltiples destinatarios
    std::vector<std::string> targets;
    std::istringstream targetStream(target);
    std::string singleTarget;
    while (std::getline(targetStream, singleTarget, ',')) {
        targets.push_back(singleTarget);
    }

    for (size_t i = 0; i < targets.size(); ++i) {
        if (targets[i][0] == '#') {  // Destino es un canal
            std::cout << "[DEBUG] NOTICE addressed to channel: " << targets[i] << std::endl;
            Channel* channel = srv.getChannelManager().getChannelByName(targets[i]);
            if (channel) {
                std::string formatted_msg = ":" + client.getFullIdentifier() + " NOTICE " + channel->getName() + " :" + msg + "\r\n";
                channel->broadcast(formatted_msg, client.getNickname()); // Excluir al emisor
            } else {
                const char* error_msg = "ERROR: Channel not found.\r\n";
                send(client.getFd(), error_msg, strlen(error_msg), 0);
                std::cout << "[DEBUG] Error: Channel not found -> " << targets[i] << std::endl;
            }
        } else {  // Destino es un cliente
            std::cout << "[DEBUG] NOTICE addressed to user: " << targets[i] << std::endl;
            Client* recipient = srv.getClientManager().getClientByNickname(targets[i]);
            if (recipient) {
                std::string formatted_msg = ":" + client.getFullIdentifier() + " NOTICE " + targets[i] + " :" + msg + "\r\n";
                send(recipient->getFd(), formatted_msg.c_str(), formatted_msg.size(), 0);
            }
            else {
                const char* error_msg = "ERROR: Nickname not found.\r\n";
                send(client.getFd(), error_msg, strlen(error_msg), 0);
                std::cout << "[DEBUG] Error: Nickname not found -> " << targets[i] << std::endl;
            }
        }
    }
}
