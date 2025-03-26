/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BOT.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: patri <patri@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:40:26 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/26 16:01:08 by patri            ###   ########.fr       */
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
    // Verificar autenticación del usuario
    if (!client.isAuthenticated()) {
        const char* error_msg = "Warning: Falta completar la autenticación.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;  // Asegúrate de que la función termine aquí si no está autenticado.
    }

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
    std::string subcommand;
    iss >> subcommand;

    // Piedra, Papel o Tijera
    if (subcommand == "rps") 
    {
        std::string user_move;
        iss >> user_move;

        if (user_move.empty()) 
        {
            const char* error_msg = "ERROR: Debes especificar tu jugada (piedra, papel o tijera).\n";
            send(client.getFd(), error_msg, strlen(error_msg), 0);
            return;
        }

        // Convertir la jugada del usuario a minúsculas
        std::transform(user_move.begin(), user_move.end(), user_move.begin(), ::tolower);

        // Validar la jugada
        if (user_move != "piedra" && user_move != "papel" && user_move != "tijera") 
        {
            const char* error_msg = "ERROR: Jugada inválida. Usa 'piedra', 'papel' o 'tijera'.\n";
            send(client.getFd(), error_msg, strlen(error_msg), 0);
            return;
        }

        // Generar la jugada del bot (0: piedra, 1: papel, 2: tijera)
        std::string options[3] = { "piedra", "papel", "tijera" };
        std::string bot_move = options[rand() % 3];

        // Determinar el resultado
        std::string result;
        if (user_move == bot_move) 
            result = "Empate!";
        else if ((user_move == "piedra" && bot_move == "tijera") ||
                 (user_move == "papel" && bot_move == "piedra") ||
                 (user_move == "tijera" && bot_move == "papel")) 
            result = "Ganaste!";
        else 
            result = "Perdiste!";

        // Enviar respuesta al cliente
        std::string response = "Bot: Yo escogí " + bot_move + ". " + result + "\n";
        send(client.getFd(), response.c_str(), response.length(), 0);
    } 
    // Coinflip (cara o cruz)
    else if (subcommand == "coinflip") 
    {
        std::string result = (rand() % 2 == 0) ? "Salió cara!" : "Salió cruz!";
        std::string response = "Bot: " + result + "\n";
        send(client.getFd(), response.c_str(), response.length(), 0);
    } 
    // Subcomando no reconocido
    else
    {
        const char* error_msg = "ERROR: Subcomando no reconocido. Usa !bot para ver las opciones.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
    }
}
