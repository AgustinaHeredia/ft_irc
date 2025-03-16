/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 10:59:09 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/16 19:19:38 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

void CommandHandler::handleKick(Server &srv, Client &client, const std::string &message) {
    std::cout << "[DEBUG] Recibido KICK: " << message << std::endl;
    std::istringstream iss(message);
    std::string channel_name, target_nick, reason;

    iss >> channel_name >> target_nick;
    std::getline(iss >> std::ws, reason); // Captura el motivo si existe

    std::cout << "[DEBUG] Comando KICK recibido: " << message << std::endl;
    std::cout << "[DEBUG] Canal: " << channel_name << ", Usuario: " << target_nick << ", Motivo: " << reason << std::endl;

	if (!client.isAuthenticated()) {
        const char* error_msg = "Warning: Falta completar la autenticación.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
		std::cout << "[DEBUG] Cliente no autenticado intentó KICK " << std::endl;
        return;
    }

    //  Verificar si el canal es válido
    if (channel_name.empty() || channel_name[0] != '#') {
        const char* error_msg = "ERROR: Nombre de canal inválido. Debe comenzar con '#'.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Buscar el canal
    Channel* channel = srv.getChannelManager().getChannelByName(channel_name);
    if (!channel) {
        const char* error_msg = "ERROR: Canal no encontrado.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Verificar si el usuario que ejecuta `KICK` está en el canal
    if (!channel->isClientInChannel(client)) {
        const char* error_msg = "ERROR: No estás en este canal.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Verificar si el usuario tiene permisos para expulsar (es operador)
    if (!channel->isOperator(client)) {
        const char* error_msg = "ERROR: No tienes permisos para expulsar usuarios.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Buscar al usuario objetivo
    Client *target = srv.getClientManager().getClientByNickname(target_nick);
    if (!target || !channel->isClientInChannel(*target)) {
        const char* error_msg = "ERROR: El usuario no está en este canal.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Notificar a todos en el canal sobre la expulsión
    std::string kick_msg = ":" + client.getNickname() + " KICK " + channel_name + " " + target_nick + " :" + (reason.empty() ? "Expulsado" : reason) + "\n";
    channel->broadcast(kick_msg);

    //  Expulsar al usuario del canal
    std::string target_msg = "Has sido expulsado de " + channel_name + ". Motivo: " + reason + "\n";
    send(target->getFd(), target_msg.c_str(), target_msg.size(), 0);

    std::cout << "[DEBUG] Usuario " << target_nick << " ha sido expulsado de " << channel_name << std::endl;
}
