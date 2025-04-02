/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:14 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/02 12:54:42 by pquintan         ###   ########.fr       */
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
	
    if (client.getAuthState() != Client::AUTH_COMPLETE) {
        std::vector<std::string> params;
        params.push_back("Authentication required");
        server.sendReply(Reply::ERR_NOTREGISTERED, client, params);
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
        handleUserCommand(server, client, user);
    } else if (cmd == "QUIT") {
    std::cout << "QUIT command received" << std::endl;
    std::string reason = command.substr(cmd.length());
    reason.erase(0, reason.find_first_not_of(" \t\r\n"));
    
    // 1. Marcar como zombie inmediatamente
    client.setZombie(true);
    
    // 2. Notificar canales (usando solo el nickname)
    server.getChannelManager().notifyClientQuit(client.getNickname(), reason);
    
    // 3. Cerrar el socket directamente
    close(client.getFd());
    
    // 4. Programar eliminación segura
    server.getClientManager().queueForRemoval(&client);
    
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
            send(client.getFd(), "ERROR: Unknown DCC command.\r\n", 30, 0);
        }
    } else if (cmd == "CAP") {
        // Ignoramos cualquier comando CAP que llegue desde el cliente
        std::string cap_response = server.getServerName() + " CAP * LS :\r\n"; // Lista vacía
        send(client.getFd(), cap_response.c_str(), cap_response.size(), 0);
        std::cout << "✅ [DEBUG] Responded to CAP LS" << std::endl;
    } else if (cmd == "PING") {
		std::string params;
        std::getline(iss, params);
		params.erase(0, params.find_first_not_of(" \t\r\n"));
		std::string response = "PONG " + params + "\r\n";
		send(client.getFd(), response.c_str(), response.size(), 0);
		std::cout << "✅ [DEBUG] Responded to PING: " << response;
		return;
	} else {
        std::cout << "Unknown command." << std::endl;
        const char* warning_msg = "Warning: Unknown command.\r\n";
        send(client.getFd(), warning_msg, strlen(warning_msg), 0);
    }
}
