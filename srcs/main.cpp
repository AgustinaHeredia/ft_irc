/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 11:59:25 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/02 19:09:34 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/utils.hpp"
#include "server/Server.hpp"
#include <iostream>
#include <csignal>


bool g_shutdown = false;

void signalHandler(int signum) {
    g_shutdown = true;
    std::cout << "Señal recibida: " << signum << ", cerrando servidor..." << std::endl;
}

int main(int argc, char** argv) {
    // Validar argumentos
    if (argc != 3) {
        std::cerr << "Uso: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);  // Convertir argumento a entero
    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Puerto inválido. Debe estar entre 1 y 65535." << std::endl;
        return 1;
    }

    std::string password = argv[2];

    // Manejo de señales
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try {
        Server server(port, password);
        server.start();

        while (!g_shutdown) {
            server.process();
        }

        server.stop();
    } catch (const std::exception& e) {
        std::cerr << "Error crítico: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
