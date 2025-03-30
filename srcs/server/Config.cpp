/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agusheredia <agusheredia@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 12:52:03 by agusheredia       #+#    #+#             */
/*   Updated: 2025/03/30 17:19:22 by agusheredia      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config() {
	std::ifstream file(FILE, std::ifstream::in);
	if (!file) throw std::runtime_error("Error opening config file");

	std::string line;
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string key, value;
		if (std::getline(ss, key, '=') && std::getline(ss, value)) {
			settings[key] = value;
		}
	}
}

Config::~Config() {}

Config	&Config::operator=(const Config &src)
{
	this->settings = src.settings;

	return (*this);
}

const std::map<std::string, std::string>	&Config::get_map(void) const
{ return (this->settings); }

void	Config::set(const std::string &key, const std::string &val)
{ this->settings[key] = val; }

std::string Config::get(const std::string &key, const std::string &defaultValue) const {
	std::map<std::string, std::string>::const_iterator it = settings.find(key);
	return (it != settings.end()) ? it->second : defaultValue;
}
