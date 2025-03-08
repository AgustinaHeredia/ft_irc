/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:39:53 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/09 00:01:35 by agusheredia      ###   ########.fr       */
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
    std::cout << "[DEBUG] Recibido PRIVMSG: " << message << std::endl;

    // Verificar si el cliente está autenticado
    if (!client.isAuthenticated()) {
        send(client.getFd(), "ERROR: No autenticado.\n", 24, 0);
        std::cout << "[DEBUG] Cliente no autenticado, rechazando PRIVMSG" << std::endl;
        return;
    }

    std::istringstream iss(message);
    std::string target, msg;

    // Extraer el destinatario
    iss >> target;

    // Verificar si el destinatario está vacío
    if (target.empty()) {
        send(client.getFd(), "ERROR: Uso incorrecto de PRIVMSG en target.\n", 44, 0);
        std::cout << "[DEBUG] Error: Target vacío en PRIVMSG" << std::endl;
        return;
    }

    // Extraer el mensaje completo después del target
    if (iss.peek() == ':') {
        iss.get(); // Eliminar el carácter ':'
    }
    std::getline(iss, msg);

    // Verificar si el mensaje está vacío
    if (msg.empty()) {
        send(client.getFd(), "ERROR: Uso incorrecto de PRIVMSG. Mensaje vacío.\n", 50, 0);
        std::cout << "[DEBUG] Error: Mensaje vacío en PRIVMSG" << std::endl;
        return;
    }

    std::cout << "[DEBUG] PRIVMSG de " << client.getNickname() << " para " << target << ": " << msg << std::endl;

    // Manejo de múltiples destinatarios
    std::vector<std::string> targets;
    std::istringstream targetStream(target);
    std::string singleTarget;
    while (std::getline(targetStream, singleTarget, ',')) {
        targets.push_back(singleTarget);
    }

    for (size_t i = 0; i < targets.size(); ++i) {
        if (targets[i][0] == '#') {
            std::cout << "[DEBUG] PRIVMSG dirigido a canal: " << targets[i] << std::endl;
            Channel* channel = srv.getChannelManager().getChannelByName(targets[i]);
            if (channel) {
                std::string formatted_msg = targets[i] + ":" + client.getNickname() + " :" + msg + "\n";
                channel->broadcast(formatted_msg);
                std::cout << "[DEBUG] Mensaje enviado a canal " << targets[i] << std::endl;
            } else {
                send(client.getFd(), "ERROR: Canal no encontrado.\n", 28, 0);
                std::cout << "[DEBUG] Error: Canal no encontrado -> " << targets[i] << std::endl;
            }
        } else {
            std::cout << "[DEBUG] PRIVMSG dirigido a usuario: " << targets[i] << std::endl;
            Client* recipient = srv.getClientManager().getClientByNickname(targets[i]);
            if (recipient) {
                std::string formatted_msg = client.getNickname() + ": " + msg + "\n";
                
                // Verificar que el socket está listo para escritura
                pollfd pfd;
                pfd.fd = recipient->getFd();
                pfd.events = POLLOUT;
                pfd.revents = 0;

                if (poll(&pfd, 1, 100) > 0 && (pfd.revents & POLLOUT)) {
                    send(recipient->getFd(), formatted_msg.c_str(), formatted_msg.size(), 0);
                    std::cout << "[DEBUG] Mensaje enviado a usuario " << targets[i] << std::endl;
                } else {
                    send(client.getFd(), "ERROR: No se puede enviar mensaje en este momento.\n", 50, 0);
                    std::cout << "[DEBUG] Error: No se pudo enviar mensaje a " << targets[i] << std::endl;
                }
            } else {
                send(client.getFd(), "ERROR: Nickname no encontrado.\n", 30, 0);
                std::cout << "[DEBUG] Error: Nickname no encontrado -> " << targets[i] << std::endl;
            }
        }
    }
}
