/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: patri <patri@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:09 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/18 11:29:16 by patri            ###   ########.fr       */
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
		Server &server;
		void		botSendResponse(Client& client, const std::string& msg);
		void		botRPSLogic(Client& client, const std::string& user_choice);
		std::string	botGenerateRPSChoice();
	public:
		CommandHandler(Server &server);
		void handleCommand(Client &client, const std::string &command);

		void handleNickCommand(Server &srv, Client &client, const std::string &nick);
		void handleUserCommand(Client &client, const std::string &user);
		void handlePrivmsg(Server &srv, Client &client, const std::string &message);
		void handleJoin(Server &srv, Client &client, const std::string &channel_name);
		void handlePart(Server &srv, Client &client, const std::string &channel_name);
		void handleKick(Server &srv, Client &client, const std::string &message);
		void handleTopic(Server &srv, Client &client, const std::string &message);
		void handleMode(Server& srv, Client &client, const std::string &message);
		void handleInvite(Server& srv, Client& client, const std::string& message);
		void handleWho(Server &srv, Client &client, const std::string& message);
		void handleNotice(Server &srv, Client &client, const std::string &message);
		void handleBot(Client &client, const std::string &message);
};

#endif
