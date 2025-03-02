/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:42 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/02 19:17:59 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//gestionará las conexiones y los sockets

#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <map>
# include <poll.h>  // Necesario para manejar poll()
# include "../clients/Client.hpp"
# include "../channel/Channel.hpp"
# include "../comands/CommandHandler.hpp"
# include "ClientManager.hpp"
# include "ChannelManager.hpp"

# define MAX_CLIENTS 10

class Server {
	private:
		int server_fd; // Descriptor de socket del servidor
		int port;
		std::string password;
		std::vector<pollfd> fds; // Vector para manejar múltiples clientes con poll()
		CommandHandler commandHandler;

		ClientManager clientManager;
    	ChannelManager channelManager;
	
		void acceptClients();
		
	public:
		Server(int port, const std::string& password);
		~Server();
		void start();
		void stop();
		void process();
		void handleCommand(Client& client, const std::string& command);
		ClientManager& getClientManager(); // Método para obtener el ClientManager
    	ChannelManager& getChannelManager(); // Método para obtener el ChannelManager
};

#endif
