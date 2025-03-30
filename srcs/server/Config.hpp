/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 12:51:50 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/30 16:33:47 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <fstream>
# include <sstream>
# include <map>
# include <stdexcept>

# define FILE	"config/default.conf"

class Config {
private:
    std::map<std::string, std::string> settings;

	Config	&operator=(const Config &src);
	Config(const Config &src);
public:
	Config();
	~Config();
	std::string get(const std::string &key, const std::string &defaultValue = "") const;

	const std::map<std::string, std::string>	&get_map(void) const;

	void				set(const std::string &key, const std::string &val);

};

#endif
