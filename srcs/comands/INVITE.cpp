/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INVITE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 21:16:00 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/09 21:22:26 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

void CommandHandler::handleInvite(Server& srv, Client& client, const std::string& message) {
    std::istringstream iss(message);
    std::string target_nick, channel_name;
    iss >> target_nick >> channel_name;

    std::cout << "[DEBUG] Comando INVITE recibido: " << message << std::endl;

    if (target_nick.empty() || channel_name.empty() || channel_name[0] != '#') {
        const char* error_msg = "ERROR: Uso incorrecto de INVITE.\n";
		send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);
    if (!channel) {
        const char* error_msg = "ERROR: Canal no encontrado.\n";
		send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    Client *target = srv.getClientManager().getClientByNickname(target_nick);
    if (!target) {
        const char* error_msg = "ERROR: Usuario no encontrado.\n";
		send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    if (!channel->isOperator(client)) {
        const char* error_msg = "ERROR: No tienes permisos para invitar usuarios.\n";
		send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    if (channel->isClientInChannel(*target)) {
        const char* error_msg = "ERROR: El usuario ya está en el canal.\n";
		send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    channel->inviteUser(*target);
    std::string invite_msg = ":" + client.getNickname() + " INVITE " + target_nick + " " + channel_name + "\n";
    send(target->getFd(), invite_msg.c_str(), invite_msg.size(), 0);

    std::cout << "[DEBUG] " << target_nick << " ha sido invitado a " << channel_name << std::endl;
}
