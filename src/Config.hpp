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
 * Project: nut2mqtt
 * Created: September 18, 2023 19:24
 */

#pragma once

#include <string>

namespace config {

struct Nut {
  std::string sHost; // address or name (default to localhost)
  std::string sUserName;
  std::string sPassword;
  size_t nPollInterval; // seconds
};

struct Mqtt {
  std::string sId;
  std::string sHost; // address or name (port 1833)
  std::string sUserName;
  std::string sPassword;
  std::string sTopic;
};

struct Values {

  Nut nut;
  Mqtt mqtt;

};

bool Load( const std::string& sFileName, Values& );

} // namespace config