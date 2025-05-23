/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:09 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/06 11:24:02 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// procesará los comandos recibidos

#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include <string>
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"

class Server;
// class Client;
// class Channel;

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
		void handleUserCommand(Server& server, Client &client, const std::string &message);
		void handlePrivmsg(Server &srv, Client &client, const std::string &message);
		void handleJoin(Server &srv, Client &client, const std::string &channel_name);
		void handlePart(Server &srv, Client &client, const std::string &channel_name);
		void handleKick(Server &srv, Client &client, const std::string &message);
		void handleTopic(Server &srv, Client &client, const std::string &message);
		void handleMode(Server& srv, Client &client, const std::string &message);
		void handleChannelMode(Server &srv, Client &client, const std::string &target, const std::string &message);
		void handleUserMode(Server &srv, Client &client, const std::string &target, const std::string &message);
		void handleInvite(Server& srv, Client& client, const std::string& message);
		void handleWho(Server &srv, Client &client, const std::string& message);
		void handleNotice(Server &srv, Client &client, const std::string &message);
		void handleBot(Client &client, const std::string &message);
		
		std::vector<std::string> split(const std::string &s, char delimiter);
		std::string trim(const std::string &str);

};

#endif
