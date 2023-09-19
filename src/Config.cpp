/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    Config.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created: October 24, 2021 21:25
 */

#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

namespace {
  static const std::string sValue_Nut_Host( "nut_host" );
  static const std::string sValue_Nut_UserName( "nut_username" );
  static const std::string sValue_Nut_Password( "nut_password" );
  static const std::string sValue_Nut_PollInterval( "nut_poll_interval" );
  
  static const std::string sValue_Mqtt_Id( "mqtt_id" );
  static const std::string sValue_Mqtt_Host( "mqtt_host" );
  static const std::string sValue_Mqtt_UserName( "mqtt_username" );
  static const std::string sValue_Mqtt_Password( "mqtt_password" );
  static const std::string sValue_Mqtt_Topic( "mqtt_topic" );

  template<typename T>
  bool parse( const std::string& sFileName, po::variables_map& vm, const std::string& name, T& dest ) {
    bool bOk = true;
    if ( 0 < vm.count( name ) ) {
      dest = vm[name].as<T>();
      BOOST_LOG_TRIVIAL(info) << name << " = " << dest;
    }
    else {
      BOOST_LOG_TRIVIAL(error) << sFileName << " missing '" << name << "='";
      bOk = false;
    }
  return bOk;
  }
}

namespace config {

bool Load( const std::string& sFileName, Values& values ) {

  bool bOk( true );

  try {

    std::string sDateHistory;
    std::string sDateTrading;
    unsigned int nDaysFront;
    unsigned int nDaysBack;

    po::options_description config( "Nut2MQTT Config" );
    config.add_options()
      ( sValue_Nut_Host.c_str(), po::value<std::string>( &values.nut.sHost ), "nut host address or name" )
      ( sValue_Nut_UserName.c_str(), po::value<std::string>( &values.nut.sUserName ), "nut username" )
      ( sValue_Nut_Password.c_str(), po::value<std::string>( &values.nut.sPassword ), "nut password" )
      ( sValue_Nut_PollInterval.c_str(), po::value<size_t>( &values.nut.sPollInterval ), "nut polling interval (seconds)" )

      ( sValue_Mqtt_Id.c_str(), po::value<std::string>( &values.mqtt.sId ), "mqtt client id" )
      ( sValue_Mqtt_Host.c_str(), po::value<std::string>( &values.mqtt.sHost ), "mqtt host address or name" )
      ( sValue_Mqtt_UserName.c_str(), po::value<std::string>( &values.mqtt.sUserName ), "mqtt username" )
      ( sValue_Mqtt_Password.c_str(), po::value<std::string>( &values.mqtt.sPassword ), "mqtt password" )
      ( sValue_Mqtt_Topic.c_str(), po::value<std::string>( &values.mqtt.sTopic ), "mqtt topic" )
      ;
    po::variables_map vm;
    //po::store( po::parse_command_line( argc, argv, config ), vm );

    std::ifstream ifs( sFileName.c_str() );

    if ( !ifs ) {
      BOOST_LOG_TRIVIAL(error) << "nut2mqtt config file " << sFileName << " does not exist";
      bOk = false;
    }
    else {
      po::store( po::parse_config_file( ifs, config), vm );

      bOk &= parse<std::string>( sFileName, vm, sValue_Nut_Host, values.nut.sHost );
      bOk &= parse<std::string>( sFileName, vm, sValue_Nut_UserName, values.nut.sUserName );
      bOk &= parse<std::string>( sFileName, vm, sValue_Nut_Password, values.nut.sPassword );
      bOk &= parse<size_t>( sFileName, vm, sValue_Nut_PollInterval, values.nut.sPollInterval );

      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_Id, values.mqtt.sId );
      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_Host, values.mqtt.sHost );
      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_UserName, values.mqtt.sUserName );
      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_Password, values.mqtt.sPassword );
      bOk &= parse<std::string>( sFileName, vm, sValue_Mqtt_Topic, values.mqtt.sTopic );
    }

  }
  catch( std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << sFileName << " parse error: " << e.what();
    bOk = false;
  }

  return bOk;

}

} // namespace config
