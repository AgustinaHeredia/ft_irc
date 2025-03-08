/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:14 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/08 11:11:13 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../server/Server.hpp"
#include "../server/ClientManager.hpp"
#include <iostream>
#include <sstream>

CommandHandler::CommandHandler(Server& server) : server(server) {}

void CommandHandler::handleCommand(Client& client, const std::string& command) {
    std::cout << "Manejando comando: " << command << std::endl;
	std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if (cmd == "NICK") {
        std::string nick;
        iss >> nick;
        handleNickCommand(server, client, nick);
    } else if (cmd == "USER") {
        std::string user;
        iss >> user;
        handleUserCommand(client, user);
    } else if (cmd == "QUIT") {
        std::cout << "Comando QUIT recibido" << std::endl;
        server.getClientManager().removeClient(&client);
    } else if (cmd == "PRIVMSG") {
        std::string message;
        std::getline(iss, message);
        handlePrivmsg(server, client, message);
    } else if (cmd == "JOIN") {
        std::string channel_name;
        iss >> channel_name;
        handleJoin(server, client, channel_name);
    } else if (cmd == "PART") {
		std::string channel_name;
        iss >> channel_name;
        handlePart(server, client, channel_name);
	} else if (cmd == "KICK") {
        std::string message;
        std::getline(iss, message);
        handleKick(server, client, message); 
	} else {
        std::cout << "Comando desconocido" << std::endl;
    }
}
