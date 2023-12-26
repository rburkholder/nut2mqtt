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
 * File:    Loop.hpp
 * Author:  raymond@burkholder.net
 * Project: Nut2MQTT
 * Created: October 8, 07:56:05
 */

#include <set>
#include <memory>
#include <string>

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/executor_work_guard.hpp>

namespace config {
  class Values;
}

namespace nut {
  class TcpClient;
}

namespace ou {
  class Mqtt;
}


namespace asio = boost::asio; // from <boost/asio/context.hpp>

class Loop {
public:
  Loop( const config::Values&, asio::io_context& );
  ~Loop();
protected:
private:

  const config::Values& m_choices;
  asio::io_context& m_io_context;

  std::unique_ptr<ou::Mqtt> m_pMqtt;
  std::unique_ptr<nut::TcpClient> m_pNutClient;

  using work_guard_t = asio::executor_work_guard<asio::io_context::executor_type>;
  using pWorkGuard_t = std::unique_ptr<work_guard_t>;

  pWorkGuard_t m_pWorkGuard;

  using setName_t = std::set<std::string>;
  using vValue_t = std::vector<std::string>;

  setName_t m_setDeviceNames;

  asio::signal_set m_signals;
  asio::steady_timer m_timerPollInterval;

  void Poll( bool bAll, bool bEnumerate );
  void Signals( const boost::system::error_code&, int );

};