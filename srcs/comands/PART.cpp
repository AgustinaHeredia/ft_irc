/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PART.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:40:36 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/08 10:37:16 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

void CommandHandler::handlePart(Server &srv, Client &client, const std::string &channel_name) {
    if (channel_name.empty() || channel_name[0] != '#') {
        const char* error_msg = "ERROR: Nombre de canal inválido. Debe comenzar con '#'.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);
    if (!channel) {
        const char* error_msg = "ERROR: Canal no encontrado.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Verificar si el cliente está en el canal
    if (!channel->isClientInChannel(client)) {
        const char* error_msg = "ERROR: No estás en este canal.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Notificar a los demás que el usuario salió
    std::string part_msg = ":" + client.getNickname() + " PART " + channel_name + "\n";
    channel->broadcast(part_msg);

    //  Remover al cliente del canal
    channel->removeClient(client);
    const char* success_msg = "Has salido del canal.\n";
    send(client.getFd(), success_msg, strlen(success_msg), 0);

    std::cout << "[DEBUG] Cliente " << client.getNickname() << " salió de " << channel_name << std::endl;

    //  Si el canal está vacío, eliminarlo
    if (channel->isEmpty()) {
        srv.getChannelManager().removeChannel(channel_name);
        std::cout << "[DEBUG] Canal " << channel_name << " eliminado porque quedó vacío." << std::endl;
    }
}
