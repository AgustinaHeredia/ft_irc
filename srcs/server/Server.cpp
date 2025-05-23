/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 19:07:11 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/04 11:33:25 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sstream>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <arpa/inet.h>
#include <algorithm>
#include <cctype>

#define BUFFER_SIZE 1024
#define BACKLOG 10

Server::Server(int puerto, const std::string &pwd, const Config &conf)
    : port(puerto), config(conf), password(pwd), server_name("irc.ircserv.com"), commandHandler(*this)
{}

void Server::start() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        throw std::runtime_error("Error creating socket");
    }
    std::cout << "✅ Server socket created." << std::endl;

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        throw std::runtime_error("Error configuring socket");
    }
    std::cout << "🔧 Socket configured for reuse address." << std::endl;

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        throw std::runtime_error("Error binding socket");
    }
    std::cout << "🔗 Socket bound to port " << port << std::endl;

    if (listen(server_fd, BACKLOG) == -1) {
        throw std::runtime_error("Error putting socket in listening mode");
    }
    std::cout << "👂 Server listening on port " << port << std::endl;

    pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN;
    fds.push_back(server_pollfd);
    std::cout << "🚀 Server started successfully on port " << port << std::endl;
}

void Server::process() {
    std::string value = config.get("timeout", "5000");
    long timeout = std::strtol(value.c_str(), NULL, 10);

    int activity = poll(&fds[0], fds.size(), timeout);
    if (activity == -1) {
        std::cerr << "❌ Poll error: " << strerror(errno) << std::endl;
        return;
    }

    if (fds[0].revents & POLLIN) {
        std::cout << "\n🔔 New connection attempt detected" << std::endl;
        acceptClients();
    }

    // Procesar eventos en los clientes
    for (size_t i = 1; i < fds.size(); i++) {
        if (fds[i].revents & POLLIN) {
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            int bytes = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);

            if (bytes <= 0) {
                handleClientDisconnection(i);
                continue;
            }

            buffer[bytes] = '\0';
            std::cout << "📥 Raw data from fd " << fds[i].fd << ": " << buffer << std::endl;

            Client* client = clientManager.getClientByFd(fds[i].fd);
            if (!client) {
                std::cerr << "⚠️  Client not found for fd " << fds[i].fd << std::endl;
                continue;
            }

            processClientCommands(client, buffer);
        }

        // Manejar errores del socket
        if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            std::cout << "⚠️  Socket error on fd " << fds[i].fd << std::endl;
            handleClientDisconnection(i);
        }
    }
    clientManager.processRemovals();

    // Limpiar buffers antiguos (fuera del bucle de fds)
    std::vector<Client*> allClients = clientManager.getAllClients();
    for (std::vector<Client*>::iterator it = allClients.begin(); it != allClients.end(); ++it) {
        Client* client = *it;
        if (!client->getPartialCommand().empty() && client->isBufferStale()) {
            std::cout << "🕒 Clearing stale buffer for client " << client->getNickname() << std::endl;
            client->getPartialCommand().clear();
        }
    }
}

void Server::handleClientDisconnection(size_t index) {
    int fd = fds[index].fd;
    Client* client = clientManager.getClientByFd(fd);
    
    if (client) {
        // Solo notificar si no fue un QUIT voluntario
        if (!client->isZombie()) {
            std::cout << "👋 Client disconnected: " << client->getNickname() 
                      << " (fd: " << fd << ")" << std::endl;
            channelManager.notifyClientQuit(client->getNickname(), "Connection closed");
        }
        clientManager.queueForRemoval(client);
    }
    
    close(fd);
    fds.erase(fds.begin() + index);
    std::cout << "🔌 Socket " << fd << " closed" << std::endl;
}

void Server::processClientCommands(Client* client, const char* buffer) {
    if (client->isZombie()) {
        return; // Ignorar comandos de clientes marcados para eliminación
    }
    if (!clientManager.isClientValid(client)) {
        std::cout << "⚠️  Client already removed. Ignoring commands." << std::endl;
        return;
    }
    client->updateLastActivity();
    client->getPartialCommand() += buffer;

    // Debug: Mostrar estado actual
    std::cout << "🔵 [BUFFER] FD " << client->getFd() 
              << " State: " << client->getAuthState()
              << " Data: '" << client->getPartialCommand() << "'"
              << std::endl;

    // Manejo especial para PASS parcial
    if (client->getAuthState() == Client::AUTH_NONE) {
        size_t pass_pos = client->getPartialCommand().find("PASS");
        if (pass_pos != std::string::npos) {
            if (client->getPartialCommand().find("\r\n") == std::string::npos) {
                client->setExpectingContinuation(true);
                std::cout << "⏳ Waiting for complete PASS command..." << std::endl;
                return;
            }
        }
    }

    // Procesamiento normal de comandos
    size_t pos;
    while ((pos = client->getPartialCommand().find('\n')) != std::string::npos) {
        std::string cmd = client->getPartialCommand().substr(0, pos);
        // Eliminar \r si está presente al final
        if (!cmd.empty() && cmd[cmd.size() - 1] == '\r') {
            cmd.erase(cmd.size() - 1);
        }
        client->getPartialCommand().erase(0, pos + 1);
        
        cmd.erase(0, cmd.find_first_not_of(" \t\r\n"));
        cmd.erase(cmd.find_last_not_of(" \t\r\n") + 1);

        if (!cmd.empty()) {
            std::cout << "🟢 Processing command: [" << cmd << "]" << std::endl;
            if (client->getAuthState() != Client::AUTH_COMPLETE) {
                handleAuthCommands(*client, cmd);
            } else {
                commandHandler.handleCommand(*client, cmd);
            }
        }
        client->setExpectingContinuation(false);
    }

    // Limpiar buffer si lleva mucho tiempo inactivo
    if (client->isBufferStale()) {
        std::cout << "🕒 Clearing stale buffer for FD " << client->getFd() << std::endl;
        client->getPartialCommand().clear();
        client->setExpectingContinuation(false);
    }
}

void Server::acceptClients() {
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);

    if (client_fd == -1) {
        std::cerr << "❌ Accept error: " << strerror(errno) << std::endl;
        return;
    }

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
    std::cout << "➕ New client from " << ip << ":" << ntohs(client_addr.sin_port) 
              << " (fd: " << client_fd << ")" << std::endl;

    if (fds.size() >= MAX_CLIENTS) {
        std::cerr << "🚫 Max clients reached (" << MAX_CLIENTS << ")" << std::endl;
        close(client_fd);
        return;
    }

    Client* new_client = new Client(client_fd, client_addr);
    new_client->setAuthState(Client::AUTH_NONE);
    new_client->resetAuthAttempts();

    std::string password_prompt = "Enter the password in the format: PASS <password>\r\n";
    send(client_fd, password_prompt.c_str(), password_prompt.size(), 0);
    std::cout << "📤 [DEBUG] Sent password prompt to fd " << client_fd << std::endl;

    pollfd new_poll = {client_fd, POLLIN, 0};
    fds.push_back(new_poll);
    clientManager.addClient(new_client);
}

std::string getCreationDate() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

void Server::handleAuthCommands(Client& client, const std::string& cmd)
{
    std::cout << "🔐 Auth command: " << cmd << std::endl;

    size_t space_pos = cmd.find(' ');
    std::string command = cmd.substr(0, space_pos);
    std::string args = (space_pos != std::string::npos) ? cmd.substr(space_pos + 1) : "";

    std::transform(command.begin(), command.end(), command.begin(), ::toupper);

    if (command == "PASS")
    {
        std::string input_password = args.substr(0, args.find_first_of(" \r\n"));
        if (input_password == password)
        {
            client.setAuthState(Client::AUTH_PASS_OK);
            std::string success_msg = ":" + getServerName() + " NOTICE AUTH :Password accepted -> Continue with NICK and then USER \r\n";
            send(client.getFd(), success_msg.c_str(), success_msg.size(), 0);
            std::cout << "🔑 [DEBUG] Correct password from fd " << client.getFd() << std::endl;
        }
        else
        {
            handlePassword(client, input_password);
        }
    }
    else if (command == "NICK" && client.getAuthState() == Client::AUTH_PASS_OK)
    {
        commandHandler.handleNickCommand(*this, client, args);
        
        if (!client.getNickname().empty())
        {
            client.setAuthState(Client::AUTH_NICK_OK);
            std::string nick_confirm = ":" + client.getNickname() + " NICK " + client.getNickname() + " -> Now send USER in the format: USER <username> 0 * :<realname>\r\n";
            send(client.getFd(), nick_confirm.c_str(), nick_confirm.size(), 0);
            std::cout << "📝 [DEBUG] NICK accepted from fd " << client.getFd() << std::endl;
        }
    }
    else if (command == "USER" && client.getAuthState() == Client::AUTH_NICK_OK)
    {
        commandHandler.handleUserCommand(*this, client, args);
        
        if (client.getAuthState() == Client::AUTH_COMPLETE)
        {
            std::string servername = getServerName();
            std::string welcome_msg =
                ":" + servername + " 001 " + client.getNickname() + " :Welcome to the IRC server " + client.getNickname() + "!" + client.getUsername() + "@" + servername + "\r\n"
                ":" + servername + " 002 " + client.getNickname() + " :Your host is " + servername + ", running version 1.0\r\n"
                ":" + servername + " 003 " + client.getNickname() + " :This server was created " + getCreationDate() + "\r\n"
                ":" + servername + " 376 " + client.getNickname() + " :End of MOTD\r\n";

            send(client.getFd(), welcome_msg.c_str(), welcome_msg.size(), 0);
        }
        std::cout << "👤 [DEBUG] User accepted from fd " << client.getFd() << std::endl;
    }
    else
    {
        std::string error_msg;
        switch (client.getAuthState())
        {
            case Client::AUTH_NONE:
                error_msg = ":" + getServerName() + " 464 * :Password required\r\n";
                break;
            case Client::AUTH_PASS_OK:
                error_msg = ":" + getServerName() + " 461 * :You must send NICK now\r\n";
                break;
            case Client::AUTH_NICK_OK:
                error_msg = ":" + getServerName() + " 461 * :You must send USER now\r\n";
                break;
            default:
                error_msg = ":" + getServerName() + " 400 * :Invalid command sequence\r\n";
        }
        send(client.getFd(), error_msg.c_str(), error_msg.size(), 0);
        std::cout << "❌ Invalid auth sequence from fd " << client.getFd() << std::endl;
    }
}


void Server::handlePassword(Client& client, const std::string& pass) {
    if (pass == password) {
        client.setAuthState(Client::AUTH_PASS_OK);
        std::string msg = "Password accepted. Continue with NICK and then USER.\r\n";
        send(client.getFd(), msg.c_str(), msg.size(), 0);
        std::cout << "🔑 [DEBUG] Correct password from fd " << client.getFd() << std::endl;
    } else {
        client.incrementAuthAttempt();
        if (client.getAuthAttempts() >= 3) {
            std::string msg = "❌ Too many failed attempts\r\n";
            send(client.getFd(), msg.c_str(), msg.size(), 0);
            disconnectClient(client, "Too many failed attempts");
            std::cout << "🚫 Banned fd " << client.getFd() << " (bad attempts)" << std::endl;
        } else {
            std::ostringstream ss;
            ss << "⚠️  Bad password (" << (3 - client.getAuthAttempts()) << " tries left)\r\n";
            send(client.getFd(), ss.str().c_str(), ss.str().size(), 0);
            std::cout << "❌ Bad password attempt from fd " << client.getFd() << std::endl;
        }
    }
}

void Server::disconnectClient(Client& client, const std::string& reason) {

    std::string msg = "ERROR :" + reason + "\r\n";
    send(client.getFd(), msg.c_str(), msg.size(), 0);
    client.setZombie(true);
    close(client.getFd());

    for (size_t i = 0; i < fds.size(); ++i) {
        if (fds[i].fd == client.getFd()) {
            fds.erase(fds.begin() + i);
            break;
        }
    }
    clientManager.removeClient(&client);
    std::cout << "🔌 Client disconnected (fd: " << client.getFd() << "). Reason: " << reason << std::endl;
}

ClientManager &Server::getClientManager() {
    return clientManager;
}

ChannelManager &Server::getChannelManager() {
    return channelManager;
}

void Server::sendReply(const std::string &code, Client &client, const std::vector<std::string> &params) {
    std::string message;

    if (code == Reply::RPL_WELCOME) {
        message = Reply::RPL_WELCOME + " " + client.getNickname() + " " + Reply::r_RPL_WELCOME(params) + "\r\n";
    } else if (code == Reply::RPL_NOTOPIC) {
        message = Reply::RPL_NOTOPIC + " " + client.getNickname() + " " + Reply::r_RPL_NOTOPIC(params) + "\r\n";
    } else if (code == Reply::RPL_TOPIC) {
        message = Reply::RPL_TOPIC + " " + client.getNickname() + " " + Reply::r_RPL_TOPIC(params) + "\r\n";
    } else if (code == Reply::RPL_NAMREPLY) {
        message = Reply::RPL_NAMREPLY + " " + client.getNickname() + " " + Reply::r_RPL_NAMREPLY(params) + "\r\n";
    } else if (code == Reply::RPL_ENDOFNAMES) {
        message = Reply::RPL_ENDOFNAMES + " " + client.getNickname() + " " + Reply::r_RPL_ENDOFNAMES(params) + "\r\n";
    } else if (code == Reply::ERR_UNKNOWNCOMMAND) {
        message = Reply::ERR_UNKNOWNCOMMAND + " " + client.getNickname() + " " + Reply::r_ERR_UNKNOWNCOMMAND(params) + "\r\n";
    } else if (code == Reply::ERR_NEEDMOREPARAMS) {
        message = Reply::ERR_NEEDMOREPARAMS + " " + client.getNickname() + " " + Reply::r_ERR_NEEDMOREPARAMS(params) + "\r\n";
    } else if (code == Reply::ERR_NICKNAMEINUSE) {
        message = Reply::ERR_NICKNAMEINUSE + " " + client.getNickname() + " " + Reply::r_ERR_NICKNAMEINUSE(params) + "\r\n";
    } else if (code == Reply::ERR_INVALIDNICK) {
        message = Reply::ERR_INVALIDNICK + " " + client.getNickname() + " " + Reply::r_ERR_INVALIDNICK(params) + "\r\n";
    } else {
        std::cerr << "Unknown reply code: " << code << std::endl;
        return;
    }

    send(client.getFd(), message.c_str(), message.size(), 0);
    std::cout << "Sent reply: " << message;
}

Server::~Server() {
    std::cout << "Calling the Server destructor..." << std::endl;
    stop();
}

void Server::stop() {
    std::cout << "Stopping the server..." << std::endl;
    clientManager.clearAllClients();
    
    close(server_fd);
    std::cout << "Server socket closed." << std::endl;

    for (size_t i = 0; i < fds.size(); ++i) {
        if (fds[i].fd == server_fd) {
            std::cout << "Closing server socket: " << fds[i].fd << std::endl;
        } else {
            std::cout << "Closing client socket: " << fds[i].fd << std::endl;
        }
        close(fds[i].fd);
    }
    std::cout << "Number of connected clients: " << fds.size() - 1 << std::endl;
    fds.clear();
}

bool Server::isCommand(const std::string& input, const char* cmd) {
    for (size_t i = 0; i < input.length(); ++i) {
        if (toupper(input[i]) != cmd[i]) return false; // cmd debe estar en mayúsculas
    }
    return input.length() == strlen(cmd);
}

const std::string &Server::getServerName() const {
    return this->server_name;
}

void Server::setServerName(const std::string &name) {
    this->server_name = name;
}
