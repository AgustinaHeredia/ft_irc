/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: patri <patri@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:40:26 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/26 15:36:20 by patri            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

void CommandHandler::handleJoin(Server &srv, Client &client, const std::string &channel_name) {
    std::cout << "[DEBUG] Comando JOIN recibido: " << channel_name << std::endl;
    std::cout << "[DEBUG] Nombre del canal extraído: " << channel_name << std::endl;

    // Verificar si el cliente está autenticado antes de unirse a un canal
    if (!client.isAuthenticated()) {
        const char* error_msg = "ERROR: Debes autenticarte antes de unirte a un canal.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        std::cout << "[DEBUG] Cliente no autenticado intentó unirse a " << channel_name << std::endl;
        return;
    }

    // Verificar si el nombre del canal es válido
    if (channel_name.empty() || channel_name[0] != '#') {
        const char* error_msg = "ERROR: Nombre de canal inválido. Debe comenzar con '#'.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        std::cout << "[DEBUG] Error: Nombre de canal inválido -> " << channel_name << std::endl;
        return;
    }

    // Buscar o crear el canal
    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);

    if (!channel) {
        std::cout << "[DEBUG] Canal " << channel_name << " no encontrado, creando nuevo canal." << std::endl;
        srv.getChannelManager().addChannel(Channel(channel_name));
        channel = srv.getChannelManager().getChannelByName(channel_name);
        // Al crear el canal, asignar al cliente como operador.
        channel->setOperator(client, true);
        std::cout << "[DEBUG] " << client.getNickname() << " asignado como operador en " << channel_name << std::endl;
    }

    // Verificar si el cliente ya está en el canal
    if (channel->isClientInChannel(client)) {
        const char* error_msg = "ERROR: Ya estás en este canal.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        std::cout << "[DEBUG] Cliente " << client.getNickname() << " ya está en " << channel_name << std::endl;
        return;
    }

    // Agregar al cliente al canal
    channel->addClient(client);
    std::cout << "[DEBUG] Cliente " << client.getNickname() << " se unió a " << channel_name << std::endl;

    // Notificar al canal que el cliente se unió
    std::string join_msg = ":" + client.getNickname() + " JOIN " + channel_name + "\n";
    channel->broadcast(join_msg);

    // Si el canal es solo por invitación, verificar la invitación
    if (channel->isInviteOnly() && !channel->isUserInvited(client)) {
        send(client.getFd(), "ERROR: Este canal es solo por invitación.\n", 40, 0);
        return;
    }
    channel->removeInvitedUser(client);
}
