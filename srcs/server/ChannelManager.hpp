/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelManager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pquintan <pquintan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:21:30 by agusheredia       #+#    #+#             */
/*   Updated: 2025/04/01 19:33:48 by pquintan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_MANAGER_HPP
# define CHANNEL_MANAGER_HPP

# include <vector>
#include "../channel/Channel.hpp"

class ChannelManager {
	private:
		std::vector<Channel> channels;
	public:
		void addChannel(const Channel &channel);
		void removeChannel(const std::string &channelName);
		Channel* getChannelByName(const std::string &name);
		void removeClientFromChannels(Client *client);

		void notifyClientQuit(const std::string& nickname, const std::string& reason);

};

#endif
