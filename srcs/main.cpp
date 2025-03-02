/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 11:59:25 by agusheredia       #+#    #+#             */
/*   Updated: 2025/02/23 17:16:58 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/utils.hpp"
#include "server/Server.hpp"

int main() 
{
    Server server;
    server.start();

    std::cout << "Servidor iniciado. Presiona Enter para detener." << std::endl;
    std::cin.get();

    server.stop();
    std::cout << "Servidor detenido." << std::endl;

    return 0;
}
