/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelManager.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:21:47 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/31 16:32:01 by pquintan         ###   ########.fr       */
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
        if (it->isClientInChannel(*client)) {
            it->removeClient(*client);
        }
    }
}
