/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INVITE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/09 21:16:00 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/01 16:33:55 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

void CommandHandler::handleInvite(Server& srv, Client& client, const std::string& message) {
    std::istringstream iss(message);
    std::string target_nick, channel_name;
    iss >> target_nick >> channel_name;

    std::cout << "[DEBUG] INVITE command received: " << message << std::endl;

	if (client.isAuthenticated()) {
        
		if (target_nick.empty() || channel_name.empty() || channel_name[0] != '#') {
			const char* error_msg = "ERROR: Incorrect use of INVITE.\r\n";
			send(client.getFd(), error_msg, strlen(error_msg), 0);
			return;
		}

		Channel* channel = srv.getChannelManager().getChannelByName(channel_name);
		if (!channel) {
			const char* error_msg = "ERROR: Channel not found.\r\n";
			send(client.getFd(), error_msg, strlen(error_msg), 0);
			return;
		}

		Client *target = srv.getClientManager().getClientByNickname(target_nick);
		if (!target) {
			const char* error_msg = "ERROR: User not found.\r\n";
			send(client.getFd(), error_msg, strlen(error_msg), 0);
			return;
		}

		if (!channel->isOperator(client)) {
			const char* error_msg = "ERROR: You do not have permission to invite users.\r\n";
			send(client.getFd(), error_msg, strlen(error_msg), 0);
			return;
		}

		if (channel->isClientInChannel(*target)) {
			const char* error_msg = "ERROR: The user is already in the channel.\r\n";
			send(client.getFd(), error_msg, strlen(error_msg), 0);
			return;
		}

		channel->inviteUser(*target);
		std::string invite_msg = ":" + client.getNickname() + " INVITE " + target_nick + " " + channel_name + "\r\n";
		send(target->getFd(), invite_msg.c_str(), invite_msg.size(), 0);

		std::cout << "[DEBUG] " << target_nick << " has been invited to " << channel_name << std::endl;
	} else {
		const char* warning_msg = "Warning: Authentication is missing.\r\n";
		send(client.getFd(), warning_msg, strlen(warning_msg), 0);
	}
}
