/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BOT.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: patri <patri@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:40:26 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/18 11:43:37 by patri            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../server/Server.hpp"
#include "../clients/Client.hpp"
#include "../channel/Channel.hpp"
#include <cstdlib> // rand srand
#include <ctime> // time
#include <sstream>
#include <algorithm>


void CommandHandler::handleBot(Client &client, const std::string &message) 
{
    // Si no hay argumentos, mostrar el menú de opciones disponibles
    if (message.empty()) 
    {
        const char* menu = 
            "Opciones del Bot:\n"
            " - !bot rps <piedra|papel|tijera> : Juega Piedra, Papel o Tijera.\n"
            " - !bot coinflip : Lanza una moneda (cara o cruz).\n";
        send(client.getFd(), menu, strlen(menu), 0);
        return;
    }

    std::istringstream iss(message);
    std::string subcomando;
    iss >> subcomando;

    // Piedra, Papel o Tijera
    if (subcomando == "rps") 
    {
        std::string jugada_usuario;
        iss >> jugada_usuario;

        if (jugada_usuario.empty()) 
        {
            const char* error_msg = "ERROR: Debes especificar tu jugada (piedra, papel o tijera).\n";
            send(client.getFd(), error_msg, strlen(error_msg), 0);
            return;
        }

        // Convertir la jugada del usuario a minúsculas
        std::transform(jugada_usuario.begin(), jugada_usuario.end(), jugada_usuario.begin(), ::tolower);

        // Validar la jugada
        if (jugada_usuario != "piedra" && jugada_usuario != "papel" && jugada_usuario != "tijera") 
        {
            const char* error_msg = "ERROR: Jugada inválida. Usa 'piedra', 'papel' o 'tijera'.\n";
            send(client.getFd(), error_msg, strlen(error_msg), 0);
            return;
        }

        // Generar la jugada del bot (0: piedra, 1: papel, 2: tijera)
        std::string opciones[3] = { "piedra", "papel", "tijera" };
        std::string jugada_bot_str = opciones[rand() % 3];

        // Determinar el resultado
        std::string resultado;
        if (jugada_usuario == jugada_bot_str) 
            resultado = "Empate!";
        else if ((jugada_usuario == "piedra" && jugada_bot_str == "tijera") ||
                 (jugada_usuario == "papel" && jugada_bot_str == "piedra") ||
                 (jugada_usuario == "tijera" && jugada_bot_str == "papel")) 
            resultado = "Ganaste!";
        else 
            resultado = "Perdiste!";

        // Enviar respuesta al cliente
        std::string respuesta = "Bot: Yo escogí " + jugada_bot_str + ". " + resultado + "\n";
        send(client.getFd(), respuesta.c_str(), respuesta.length(), 0);
    } 
    // Coinflip (cara o cruz)
    else if (subcomando == "coinflip") 
    {
        std::string resultado = (rand() % 2 == 0) ? "Salió cara!" : "Salió cruz!";
        std::string respuesta = "Bot: " + resultado + "\n";
        send(client.getFd(), respuesta.c_str(), respuesta.length(), 0);
    } 
    // Subcomando no reconocido
    else 
    {
        const char* error_msg = "ERROR: Subcomando no reconocido. Usa !bot para ver las opciones.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
    }
}
