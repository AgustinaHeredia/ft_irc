/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BOT.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 13:40:26 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/29 11:08:20 by agusheredia      ###   ########.fr       */
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
        const char* error_msg = "Warning: Authentication is missing.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
        return;  // Asegúrate de que la función termine aquí si no está autenticado.
    }

    // Si no hay argumentos, mostrar el menú de opciones disponibles
    if (message.empty()) 
    {
        const char* menu = 
            "Bot Options:\n"
            " - !bot rps <rock|paper|scissors> : Play Rock, Paper, Scissors.\n"
            " - !bot coinflip : Flip a coin (heads or tails).\n";
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
            const char* error_msg = "ERROR: You must specify your move (rock, paper, scissors).\n";
            send(client.getFd(), error_msg, strlen(error_msg), 0);
            return;
        }

        // Convertir la jugada del usuario a minúsculas
        std::transform(user_move.begin(), user_move.end(), user_move.begin(), ::tolower);

        // Validar la jugada
        if (user_move != "rock" && user_move != "paper" && user_move != "scissors") 
        {
            const char* error_msg = "ERROR: Invalid move. Use 'rock', 'paper', or 'scissors'.\n";
            send(client.getFd(), error_msg, strlen(error_msg), 0);
            return;
        }

        // Generar la jugada del bot (0: piedra, 1: papel, 2: tijera)
        std::string options[3] = { "rock", "paper", "scissors" };
        std::string bot_move = options[rand() % 3];

        // Determinar el resultado
        std::string result;
        if (user_move == bot_move) 
            result = "Draw!";
        else if ((user_move == "rock" && bot_move == "scissors") ||
                 (user_move == "paper" && bot_move == "rock") ||
                 (user_move == "scissors" && bot_move == "paper")) 
            result = "You won!";
        else 
            result = "You lost!";

        // Enviar respuesta al cliente
        std::string response = "Bot: I chose " + bot_move + ". " + result + "\n";
        send(client.getFd(), response.c_str(), response.length(), 0);
    } 
    // Coinflip (cara o cruz)
    else if (subcommand == "coinflip") 
    {
        std::string result = (rand() % 2 == 0) ? "It came up heads!" : "It came up tails!";
        std::string response = "Bot: " + result + "\n";
        send(client.getFd(), response.c_str(), response.length(), 0);
    } 
    // Subcomando no reconocido
    else
    {
        const char* error_msg = "ERROR: Unrecognized subcommand. Use !bot to view options.\n";
        send(client.getFd(), error_msg, strlen(error_msg), 0);
    }
}
