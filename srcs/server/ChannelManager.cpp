/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelManager.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:21:47 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/01 19:37:43 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ChannelManager.hpp"

void ChannelManager::addChannel(const Channel &channel) {
    channels.push_back(channel);
}

void ChannelManager::removeChannel(const std::string &channelName) {
	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->getName() == channelName) {
            channels.erase(it);
            return;
        }
    }
}

Channel* ChannelManager::getChannelByName(const std::string &name) {
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i].getName() == name) {
            return &channels[i];
        }
    }
    return NULL;
}

void ChannelManager::removeClientFromChannels(Client* client) {
    for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        it->removeClient(*client); // Seguro incluso si el cliente es zombie
    }
}

void ChannelManager::notifyClientQuit(const std::string& nickname, const std::string& reason) {
    std::string msg = ":" + nickname + " QUIT :" + reason + "\r\n";
    
    for (std::vector<Channel>::iterator ch_it = channels.begin(); ch_it != channels.end(); ++ch_it) {
        if (ch_it->hasClient(nickname)) {
            ch_it->broadcast(msg);
            ch_it->removeClient(nickname); // Ahora usa el método que acepta un nickname
        }
    }
}
