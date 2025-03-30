/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 11:59:25 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/30 17:16:04 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/utils.hpp"
#include "server/Server.hpp"
#include "server/Config.hpp"
#include <iostream>
#include <csignal>
#include <ctime>
#include <cstdlib>


bool g_shutdown = false;

void signalHandler(int signum) {
    g_shutdown = true;
    std::cout << "Signal received: " << signum << ", shutting down server..." << std::endl;
}

int main(int argc, char** argv) {
    // Validar argumentos
    if (argc != 3) {
        std::cerr << "Use: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);  // Convertir argumento a entero
    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Invalid port. Must be between 1 and 65535." << std::endl;
        return 1;
    }
    srand(time(NULL));

    std::string password = argv[2];

    // Manejo de señales
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try {
        Config config;
		Server server(port, password, config);
		server.start();

        while (!g_shutdown) {
            server.process();
        }

    } catch (const std::exception& e) {
        std::cerr << "Critical error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
