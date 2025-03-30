/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:14 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/30 20:49:30 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../server/Server.hpp"
#include "../server/ClientManager.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstring>

CommandHandler::CommandHandler(Server &srv) : server(srv) {}

void CommandHandler::handleCommand(Client& client, const std::string& command) {

	// Verificar si el cliente aún existe en el ClientManager
    if (!server.getClientManager().isClientValid(&client)) {
        std::cout << "Client no longer exists. Ignoring command: " << command << std::endl;
        return;
    }
	
    // Solo procesamos comandos si el cliente está autenticado
    if (!client.isAuthenticated()) {
        std::cout << "Client not authenticated. Ignoring command: " << command << std::endl;
        return;
    }

    std::cout << "Handling command: " << command << std::endl;
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::cout << "Command: " << cmd << std::endl;

    if (cmd == "NICK") {
        std::string nick;
        iss >> nick;
        handleNickCommand(server, client, nick);
    } else if (cmd == "USER") {
        std::string user;
        std::getline(iss, user);
        handleUserCommand(client, user);
    } else if (cmd == "QUIT") {
        std::cout << "QUIT command received" << std::endl;
        server.getClientManager().removeClient(&client);
		return;
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
    } else if (cmd == "TOPIC") {
        std::string message;
        std::getline(iss, message);
        handleTopic(server, client, message);
    } else if (cmd == "MODE") {
        std::string message;
        std::getline(iss, message);
        handleMode(server, client, message);
    } else if (cmd == "INVITE") {
        std::string message;
        std::getline(iss, message);
        handleInvite(server, client, message);
    } else if (cmd == "WHO") {
        std::string message;
        std::getline(iss, message);
        handleWho(server, client, message);
    } else if (cmd == "NOTICE") {
        std::string message;
        std::getline(iss, message);
        handlePrivmsg(server, client, message);
    } else if (cmd == "!BOT") {
        std::string message;
        std::getline(iss, message);
        handleBot(client, message);
    } else if (cmd == "DCC") {
        std::string message;
        std::getline(iss, message);
        if (message.find("SEND") != std::string::npos) {
            handleDccSend(server, client, message);
        } else if (message.find("ACCEPT") != std::string::npos) {
            handleDccAccept(server, client, message);
        } else {
            send(client.getFd(), "ERROR: Unknown DCC command.\n", 30, 0);
        }
    } else if (cmd == "CAP") {
        // Ignoramos cualquier comando CAP que llegue desde el cliente
        std::string cap_end_response = "CAP END\r\n";
        send(client.getFd(), cap_end_response.c_str(), cap_end_response.size(), 0);
        std::cout << "Responding to CAP END" << std::endl;
    } else if (cmd == "PING") {
		std::string params;
        std::getline(iss, params);
		params.erase(0, params.find_first_not_of(" \t\r\n"));
		std::string response = "PONG " + params + "\r\n";
		send(client.getFd(), response.c_str(), response.size(), 0);
		std::cout << "Responded to PING: " << response;
		return;
	} else {
        std::cout << "Unknown command." << std::endl;
        const char* warning_msg = "Warning: Unknown command.\n";
        send(client.getFd(), warning_msg, strlen(warning_msg), 0);
    }
}
