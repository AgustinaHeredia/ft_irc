/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:39:53 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/29 11:43:24 by agusheredia      ###   ########.fr       */
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

void CommandHandler::handlePrivmsg(Server &srv, Client &client, const std::string &message) {
    std::cout << "[DEBUG] Received PRIVMSG: " << message << std::endl;

    // Verificar si el cliente está autenticado
    if (!client.isAuthenticated()) {
        const char* error_msg = "ERROR: Not authenticated.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        std::cout << "[DEBUG] Unauthenticated client, rejecting PRIVMSG" << std::endl;
        return;
    }

    std::istringstream iss(message);
    std::string target, msg;

    // Extraer el destinatario
    iss >> target;

    // Verificar si el destinatario está vacío
    if (target.empty()) {
        const char* error_msg = "ERROR: Incorrect use of PRIVMSG in target.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        std::cout << "[DEBUG] Error: Empty Target in PRIVMSG" << std::endl;
        return;
    }

    // Extraer el mensaje completo después del target
    if (iss.peek() == ':') {
        iss.get(); // Eliminar el carácter ':'
    }
    std::getline(iss, msg);

    // Verificar si el mensaje está vacío
    if (msg.empty()) {
        const char* error_msg = "ERROR: Incorrect use of PRIVMSG. Empty message.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        std::cout << "[DEBUG] Error: Empty message in PRIVMSG" << std::endl;
        return;
    }

	// 🔹 Si el mensaje es un DCC SEND, procesarlo
    if (msg.find("DCC SEND") == 0) {
        handleDccSend(srv, client, msg);
        return;
    }
    std::cout << "[DEBUG] PRIVMSG of " << client.getNickname() << " from " << target << ": " << msg << std::endl;

    // Manejo de múltiples destinatarios
    std::vector<std::string> targets;
    std::istringstream targetStream(target);
    std::string singleTarget;
    while (std::getline(targetStream, singleTarget, ',')) {
        targets.push_back(singleTarget);
    }

    for (size_t i = 0; i < targets.size(); ++i) {
        if (targets[i][0] == '#') {
            std::cout << "[DEBUG] PRIVMSG directed to channel: " << targets[i] << std::endl;
            Channel* channel = srv.getChannelManager().getChannelByName(targets[i]);
            if (channel) {
                std::string formatted_msg = targets[i] + ":" + client.getNickname() + " :" + msg + "\n";
                channel->broadcast(formatted_msg);
                std::cout << "[DEBUG] Message sent to channel " << targets[i] << std::endl;
            } else {
                const char* error_msg = "ERROR: Channel not found.\n";
                send(client.getFd(), error_msg, strlen(error_msg), 0);
                std::cout << "[DEBUG] Error: Channel not found. -> " << targets[i] << std::endl;
            }
        } else {
            std::cout << "[DEBUG] PRIVMSG addressed to user: " << targets[i] << std::endl;
            Client* recipient = srv.getClientManager().getClientByNickname(targets[i]);
            if (recipient) {
                std::string formatted_msg = client.getNickname() + ": " + msg + "\n";
                int bytes_sent = send(recipient->getFd(), formatted_msg.c_str(), formatted_msg.size(), 0);
                if (bytes_sent == -1) {
                    const char* error_msg = "ERROR: Message cannot be sent at this time.\n";
                    send(client.getFd(), error_msg, strlen(error_msg), 0);
                    std::cout << "[DEBUG] Error: Could not send message to " << targets[i] << std::endl;
                } else {
                    std::cout << "[DEBUG] Message sent to user " << targets[i] << std::endl;
                }
            } else {
                const char* error_msg = "ERROR: Nickname not found.\n";
                send(client.getFd(), error_msg, strlen(error_msg), 0);
                std::cout << "[DEBUG] Error: Nickname not found -> " << targets[i] << std::endl;
            }
        }
    }
}
