/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

/*
 * File:    Config.hpp
 * Author:  raymond@burkholder.net
 * Project: Nut2MQTT
 * Created: September 18, 2023 19:24
 */

#pragma once

#include <set>
#include <string>

#include <ou/mqtt/config.hpp>

namespace config {

using setName_t = std::set<std::string>;

struct Nut {
  bool bEnumerate;
  std::string sHost; // address or name (default to localhost)
  std::string sUserName;
  std::string sPassword;
  size_t nPollInterval; // seconds
  setName_t setField; // fields to send to mqtt
  setName_t setNumeric; // fields to format as numeric (subset of setField)
};

struct Values {

  Nut nut;
  ou::mqtt::Config mqtt;

};

bool Load( const std::string& sFileName, Values& );

} // namespace config