/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelManager.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:21:47 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/09 00:02:04 by agusheredia      ###   ########.fr       */
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
    return nullptr;
}
