/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PART.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:40:36 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/29 11:26:49 by agusheredia      ###   ########.fr       */
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
        const char* error_msg = "ERROR: Invalid channel name. Must begin with '#'.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

	if (!client.isAuthenticated()) {
        const char* error_msg = "Warning: Authentication is missing.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
		std::cout << "[DEBUG] Unauthenticated client attempted PART " << std::endl;
        return;
    }

    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);
    if (!channel) {
        const char* error_msg = "ERROR: Channel not found.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Verificar si el cliente está en el canal
    if (!channel->isClientInChannel(client)) {
        const char* error_msg = "ERROR: You are not on this channel.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Notificar a los demás que el usuario salió
    std::string part_msg = ":" + client.getNickname() + " PART " + channel_name + "\n";
    channel->broadcast(part_msg);

    //  Remover al cliente del canal
    channel->removeClient(client);
    const char* success_msg = "You have left the channel.\n";
    send(client.getFd(), success_msg, strlen(success_msg), 0);

    std::cout << "[DEBUG] Client " << client.getNickname() << " came out of " << channel_name << std::endl;

    //  Si el canal está vacío, eliminarlo
    if (channel->isEmpty()) {
        srv.getChannelManager().removeChannel(channel_name);
        std::cout << "[DEBUG] Channel " << channel_name << " deleted because it was empty." << std::endl;
    }
}
