/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DCC.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 20:14:46 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/01 16:33:30 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#undef FILE

void CommandHandler::handleDccSend(Server &srv, Client &sender, const std::string &msg) {
    std::istringstream iss(msg);
    std::string send_cmd, filename, ip_str, port_str, size_str;

    iss >> send_cmd >> filename >> ip_str >> port_str >> size_str;

    if (send_cmd != "SEND") {
        const char* error_msg = "ERROR: Formato incorrecto de DCC SEND.\r\n";
        send(sender.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    std::string target_nick = sender.getPartialCommand();
    Client* recipient = srv.getClientManager().getClientByNickname(target_nick);
    if (!recipient) {
        const char* error_msg = "ERROR: Usuario destinatario no encontrado.\r\n";
        send(sender.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    // Convertir puerto
    char* end_ptr;
    long port = strtol(port_str.c_str(), &end_ptr, 10);
    if (*end_ptr != '\0' || port <= 0 || port > 65535) {
        std::cerr << "[DCC] Puerto inválido." << std::endl;
        return;
    }

    // Convertir IP
    in_addr_t ip = inet_addr(ip_str.c_str());
    if (ip == INADDR_NONE) {
        std::cerr << "[DCC] IP inválida." << std::endl;
        return;
    }

    // Crear socket
    int file_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (file_sock == -1) {
        std::cerr << "[DCC] Error al crear socket." << std::endl;
        return;
    }

    sockaddr_in file_addr;
    file_addr.sin_family = AF_INET;
    file_addr.sin_addr.s_addr = INADDR_ANY;
    file_addr.sin_port = htons(port);

    if (bind(file_sock, (struct sockaddr*)&file_addr, sizeof(file_addr)) == -1) {
        std::cerr << "[DCC] Error al vincular socket: " << strerror(errno) << std::endl;
        close(file_sock);
        return;
    }

    if (listen(file_sock, 1) == -1) {
        std::cerr << "[DCC] Error al poner en escucha: " << strerror(errno) << std::endl;
        close(file_sock);
        return;
    } else {
		std::cout << "[DCC] Socket en escucha en el puerto " << port << std::endl;
	}

    std::cout << "[DCC] Esperando conexión en el puerto " << port << " para el archivo " << filename << std::endl;

    // Notificar al destinatario
    std::string dcc_msg = ":" + sender.getNickname() + " PRIVMSG " + target_nick +
                          " :DCC SEND " + filename + " " + ip_str + " " + port_str + " " + size_str + "\r\n";
    send(recipient->getFd(), dcc_msg.c_str(), dcc_msg.size(), 0);

    // Aceptar conexión
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(file_sock, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        std::cerr << "[DCC] Error al aceptar conexión: " << strerror(errno) << std::endl;
        close(file_sock);
        return;
    }

    std::cout << "[DCC] Conexión establecida con " << inet_ntoa(client_addr.sin_addr) << std::endl;

    // Abrir archivo
    std::FILE *file = fopen(filename.c_str(), "rb");
    if (!file) {
        std::cerr << "[DCC] Error al abrir el archivo: " << strerror(errno) << std::endl;
    	close(client_fd);
   		close(file_sock);
    	return;
    }

    char buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
		ssize_t bytes_sent = send(client_fd, buffer, bytes_read, 0);
		if (bytes_sent == -1) {
			std::cerr << "[DCC] Error al enviar datos: " << strerror(errno) << std::endl;
			break;
		}
	}

    std::cout << "[DCC] Archivo " << filename << " enviado a " << target_nick << std::endl;

    fclose(file);
    close(client_fd);
    close(file_sock);
}

void CommandHandler::handleDccAccept(Server& srv, Client& receiver, const std::string& msg) {
    std::istringstream iss(msg);
    std::string accept, filename, port_str, position_str;

    std::cout << "[DEBUG] Recibido PRIVMSG: " << msg << std::endl;
    iss >> accept >> filename >> port_str >> position_str;

    (void)srv;
    if (accept != "ACCEPT") {
        const char* error_msg = "ERROR: Formato incorrecto de DCC ACCEPT.\r\n";
        send(receiver.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    std::cout << "[DEBUG] port_str: " << port_str << std::endl;

    // Verificar si port_str está vacío
    if (port_str.empty()) {
        const char* error_msg = "ERROR: Puerto no especificado.\r\n";
        send(receiver.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    // Validar que port_str sea numérico
    bool isNumber = true;
    for (std::string::const_iterator it = port_str.begin(); it != port_str.end(); ++it)
    {
        if (!std::isdigit(*it))
        {
            isNumber = false;
            break;
        }
    }
    if (!isNumber) {
        const char* error_msg = "ERROR: Puerto inválido (no numérico).\r\n";
        send(receiver.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    // Convertir puerto
    char* end_ptr;
    long port = strtol(port_str.c_str(), &end_ptr, 10);
    if (*end_ptr != '\0' || port <= 0 || port > 65535) {
        const char* error_msg = "ERROR: Puerto inválido.\r\n";
        send(receiver.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    std::cout << "[DEBUG] Puerto válido: " << port << std::endl;


    // Convertir IP
    const std::string& ip_str = receiver.getPartialCommand();
    in_addr_t ip = inet_addr(ip_str.c_str());
    if (ip == INADDR_NONE) {
		std::cout << "[DEBUG] Mensaje recibido: " << msg << std::endl;
		std::cout << "[DEBUG] port_str: " << port_str << std::endl;
        const char* error_msg = "ERROR: IP inválida.\r\n";
        send(receiver.getFd(), error_msg, strlen(error_msg), 0);
        return;
    }

    // Crear socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "[DCC] Error al crear socket de cliente." << std::endl;
        return;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = ip;

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "[DCC] Error al conectarse con el remitente." << std::endl;
        close(sock);
        return;
    } else {
		std::cout << "[DCC] Conexión establecida con el remitente en " << ip_str << ":" << port << std::endl;
	}

    // Crear archivo
    int file_fd = open(filename.c_str(), O_WRONLY | O_CREAT, 0644);
    if (file_fd == -1) {
        std::cerr << "[DCC] Error al abrir archivo para escritura: " << strerror(errno) << std::endl;
        close(sock);
        return;
    }

    char buffer[4096];
    ssize_t bytes_received;
    while ((bytes_received = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        write(file_fd, buffer, bytes_received);
    }

    std::cout << "[DCC] Archivo " << filename << " recibido con éxito." << std::endl;

    close(file_fd);
    close(sock);
}
