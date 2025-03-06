/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:40:26 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/06 21:19:55 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

void CommandHandler::handleJoin(Server& srv, Client& client, const std::string& channel_name) {
    std::cout << "[DEBUG] Comando JOIN recibido: " << channel_name << std::endl;
    std::cout << "[DEBUG] Nombre del canal extraído: " << channel_name << std::endl;

    // Verificar si el cliente está autenticado antes de unirse a un canal
    if (!client.isAuthenticated()) {
        send(client.getFd(), "ERROR: Debes autenticarte antes de unirte a un canal.\n", 50, 0);
        std::cout << "[DEBUG] Cliente no autenticado intentó unirse a " << channel_name << std::endl;
        return;
    }

    // Verificar si el nombre del canal es válido
    if (channel_name.empty() || channel_name[0] != '#') {
        send(client.getFd(), "ERROR: Nombre de canal inválido. Debe comenzar con '#'.\n", 55, 0);
        std::cout << "[DEBUG] Error: Nombre de canal inválido -> " << channel_name << std::endl;
        return;
    }

    // Buscar o crear el canal
    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);

    if (!channel) {
        std::cout << "[DEBUG] Canal " << channel_name << " no encontrado, creando nuevo canal." << std::endl;
        srv.getChannelManager().addChannel(Channel(channel_name));
        channel = srv.getChannelManager().getChannelByName(channel_name);
    }

    // Verificar si el cliente ya está en el canal
    if (channel->isClientInChannel(client)) {
        send(client.getFd(), "ERROR: Ya estás en este canal.\n", 30, 0);
        std::cout << "[DEBUG] Cliente " << client.getNickname() << " ya está en " << channel_name << std::endl;
        return;
    }

    // Agregar al cliente al canal
    channel->addClient(client);
    std::cout << "[DEBUG] Cliente " << client.getNickname() << " se unió a " << channel_name << std::endl;

    // Notificar al canal que el cliente se unió
    std::string join_msg = ":" + client.getNickname() + " JOIN " + channel_name + "\n";
    channel->broadcast(join_msg);
}
