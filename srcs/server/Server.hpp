/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:42 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/02 13:09:38 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//gestionará las conexiones y los sockets

#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <cerrno>
# include <map>
# include <poll.h>
# include "../clients/Client.hpp"
# include "../channel/Channel.hpp"
# include "../commands/CommandHandler.hpp"
# include "ClientManager.hpp"
# include "ChannelManager.hpp"
# include "Config.hpp"
# include "Reply.hpp"
# include "../utils/utils.hpp"

# define MAX_CLIENTS 10

class Server {
	private:
		int port;
		const Config &config;
		std::string password;
		std::string server_name;
		int server_fd; // Descriptor de socket del servidor
		std::vector<pollfd> fds; // Vector para manejar múltiples clientes
		CommandHandler commandHandler;

		ClientManager clientManager;
    	ChannelManager channelManager;
	
		void acceptClients();

		
	public:
		Server(int puerto, const std::string &pwd, const Config &conf);
		~Server();
		void start();
		void stop();
		void process();
		ClientManager &getClientManager(); // Método para obtener el ClientManager
    	ChannelManager &getChannelManager(); // Método para obtener el ChannelManager

		Config	&getConfig(void);
		const std::string &getServerName() const;
		void setServerName(const std::string &name);
		
		void sendReply(const std::string &code, Client &client, const std::vector<std::string> &params);
		void disconnectClient(Client& client, const std::string& reason);
		void handleAuthCommands(Client& client, const std::string& command);
		void handleClientDisconnection(size_t index);
		void processClientCommands(Client* client, const char* buffer);
		void handlePassword(Client& client, const std::string& pass);

		bool isCommand(const std::string& input, const char* cmd);
};

#endif
