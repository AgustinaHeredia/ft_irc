/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:09 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/08 10:32:22 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// procesará los comandos recibidos

#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include <string>
#include "../clients/Client.hpp"

class Server;

class CommandHandler {
	private:
		Server& server;
	public:
		CommandHandler(Server& server);
		void handleCommand(Client& client, const std::string& command);

		void handleNickCommand(Server& server, Client& client, const std::string& nick);
		void handleUserCommand(Client& client, const std::string& user);
		void handlePrivmsg(Server& server, Client& client, const std::string& message);
		void handleJoin(Server& server, Client& client, const std::string &channel_name);
		void handlePart(Server &srv, Client &client, const std::string &channel_name);

};

#endif
