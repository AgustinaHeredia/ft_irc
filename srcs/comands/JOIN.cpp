/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:40:26 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/29 11:15:09 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

void CommandHandler::handleJoin(Server &srv, Client &client, const std::string &channel_name) {
    std::cout << "[DEBUG] JOIN command received: " << channel_name << std::endl;
    std::cout << "[DEBUG] Name of the extracted channel: " << channel_name << std::endl;

    // Verificar si el cliente está autenticado antes de unirse a un canal
    if (!client.isAuthenticated()) {
        const char* error_msg = "ERROR: You must authenticate before joining a channel.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        std::cout << "[DEBUG] Unauthenticated client attempted to join " << channel_name << std::endl;
        return;
    }

    // Verificar si el nombre del canal es válido
    if (channel_name.empty() || channel_name[0] != '#') {
        const char* error_msg = "ERROR: Invalid channel name. Must begin with '#'.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        std::cout << "[DEBUG] Error: Invalid channel name -> " << channel_name << std::endl;
        return;
    }

    // Buscar o crear el canal
    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);

    if (!channel) {
        std::cout << "[DEBUG] Channel " << channel_name << "not found, creating new channel." << std::endl;
        srv.getChannelManager().addChannel(Channel(channel_name));
        channel = srv.getChannelManager().getChannelByName(channel_name);
        // Al crear el canal, asignar al cliente como operador.
        channel->setOperator(client, true);
        std::cout << "[DEBUG] " << client.getNickname() << " assigned as operator in" << channel_name << std::endl;
    }

    // Verificar si el cliente ya está en el canal
    if (channel->isClientInChannel(client)) {
        const char* error_msg = "ERROR: You are already on this channel.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        std::cout << "[DEBUG] Client " << client.getNickname() << " It's already in " << channel_name << std::endl;
        return;
    }

    // Agregar al cliente al canal
    channel->addClient(client);
    std::cout << "[DEBUG] Client " << client.getNickname() << " joined " << channel_name << std::endl;

    // Notificar al canal que el cliente se unió
    std::string join_msg = ":" + client.getNickname() + " JOIN " + channel_name + "\n";
    channel->broadcast(join_msg);

    // Si el canal es solo por invitación, verificar la invitación
    if (channel->isInviteOnly() && !channel->isUserInvited(client)) {
        send(client.getFd(), "ERROR: This channel is by invitation only.\n", 40, 0);
        return;
    }
    channel->removeInvitedUser(client);
}
