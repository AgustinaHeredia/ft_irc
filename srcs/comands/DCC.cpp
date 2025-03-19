/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DCC.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 20:14:46 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/19 20:49:08 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include <cstring>
#include <sstream>
#include <iostream>

void CommandHandler::handleDccSend(Server &srv, Client &sender, const std::string &msg) {
    std::istringstream iss(msg);
    std::string dcc, send_cmd, filename, ip_str, port_str, size_str;

    iss >> dcc >> send_cmd >> filename >> ip_str >> port_str >> size_str;

    if (dcc != "DCC" || send_cmd != "SEND") {
		const char* error_msg = "ERROR: Formato incorrecto de DCC SEND.\n";
        send(sender.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    std::string target_nick = sender.getPartialCommand(); // Asumimos que el target se almacena aquí
    Client* recipient = srv.getClientManager().getClientByNickname(target_nick);
    if (!recipient) {
		const char* error_msg = "ERROR: Usuario destinatario no encontrado.\n";
        send(sender.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    //  Enviar la solicitud DCC a Bob
    std::string dcc_msg = ":" + sender.getNickname() + " PRIVMSG " + target_nick +
                          " :DCC SEND " + filename + " " + ip_str + " " + port_str + " " + size_str + "\n";
    send(recipient->getFd(), dcc_msg.c_str(), dcc_msg.size(), 0);
}

void CommandHandler::handleDccAccept(Server& srv, Client& receiver, const std::string& msg) {
    std::istringstream iss(msg);
    std::string accept, filename, port_str, position_str;

	std::cout << "[DEBUG] Recibido PRIVMSG: " << msg << std::endl;
    iss >> accept >> filename >> port_str >> position_str;

	(void)srv;
    if (accept != "ACCEPT") {
		const char* error_msg = "ERROR: Formato incorrecto de DCC ACCEPT.\n";
        send(receiver.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    std::cout << "[DCC] " << receiver.getNickname() << " ha aceptado la transferencia de " << filename << std::endl;
}
