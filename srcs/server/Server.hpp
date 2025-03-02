/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:12:42 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/02 12:58:56 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//gestionará las conexiones y los sockets

#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <map>
#include <poll.h>  // Necesario para manejar poll()
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include "../comands/CommandHandler.hpp"

class Server {
	public:
    Server();
    ~Server();
    void start();
    void stop();
    void handleCommand(Client& client, const std::string& command);
    void removeClient(Client* client);
    bool isNicknameInUse(const std::string& nick) const;
    void removeNickname(const std::string& nick);
    void addNickname(const std::string& nick, Client* client);

private:
    int server_fd;                   // Descriptor de socket del servidor
    std::vector<Client*> clients;    // Lista de clientes conectados
    std::vector<Channel> channels;   // Lista de canales de chat
    std::vector<pollfd> fds;         // Vector para manejar múltiples clientes con poll()
    std::map<std::string, Client*> nicknames;
    CommandHandler commandHandler;
    void acceptClients();
};

#endif
