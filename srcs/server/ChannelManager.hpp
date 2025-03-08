/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelManager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/02 18:21:30 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/09 00:02:19 by agusheredia      ###   ########.fr       */
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
	
};

#endif
