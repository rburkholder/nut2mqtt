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
 * Project: Nut2MQTT
 * Created: September 18, 2023 19:24
 */

#include <vector>
#include <fstream>
#include <exception>

#include <boost/log/trivial.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.hpp"

namespace {
  static const std::string sValue_Nut_Enumerate( "nut_enumerate" );
  static const std::string sValue_Nut_Host( "nut_host" );
  static const std::string sValue_Nut_UserName( "nut_username" );
  static const std::string sValue_Nut_Password( "nut_password" );
  static const std::string sValue_Nut_PollInterval( "nut_poll_interval" );

  static const std::string sValue_Mqtt_Id( "mqtt_id" );
  static const std::string sValue_Mqtt_Host( "mqtt_host" );
  static const std::string sValue_Mqtt_UserName( "mqtt_username" );
  static const std::string sValue_Mqtt_Password( "mqtt_password" );
  static const std::string sValue_Mqtt_Topic( "mqtt_topic" );

  static const std::string sValue_Nut_Field( "publish" );
  static const std::string sValue_Nut_Numeric( "numeric" );

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

  using vName_t = std::vector<std::string>;

  vName_t vField;
  vName_t vNumeric;

  bool bOk( true );

  try {

    std::string sDateHistory;
    std::string sDateTrading;
    unsigned int nDaysFront;
    unsigned int nDaysBack;

    po::options_description config( "Nut2MQTT Config" );
    config.add_options()

      ( sValue_Nut_Enumerate.c_str(), po::value<bool>( &values.nut.bEnumerate )->default_value( true ) )
      ( sValue_Nut_Host.c_str(), po::value<std::string>( &values.nut.sHost )->default_value( "localhost" ), "nut host address or name" )
      ( sValue_Nut_UserName.c_str(), po::value<std::string>( &values.nut.sUserName ), "nut username" )
      ( sValue_Nut_Password.c_str(), po::value<std::string>( &values.nut.sPassword ), "nut password" )
      ( sValue_Nut_PollInterval.c_str(), po::value<size_t>( &values.nut.nPollInterval )->default_value( 30 ), "nut polling interval (seconds)" )

      ( sValue_Mqtt_Id.c_str(), po::value<std::string>( &values.mqtt.sId ), "mqtt client id" )
      ( sValue_Mqtt_Host.c_str(), po::value<std::string>( &values.mqtt.sHost )->default_value( "localhost" ), "mqtt host address or name" )
      ( sValue_Mqtt_UserName.c_str(), po::value<std::string>( &values.mqtt.sUserName ), "mqtt username" )
      ( sValue_Mqtt_Password.c_str(), po::value<std::string>( &values.mqtt.sPassword ), "mqtt password" )
      ( sValue_Mqtt_Topic.c_str(), po::value<std::string>( &values.mqtt.sTopic )->default_value( "nut/" ), "mqtt topic" )

      ( sValue_Nut_Field.c_str(), po::value<vName_t>( &vField ), "nut field to publish" )
      ( sValue_Nut_Numeric.c_str(), po::value<vName_t>( &vNumeric ), "format field for mqtt as numeric" )
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

      bOk &= parse<bool>( sFileName, vm, sValue_Nut_Enumerate, values.nut.bEnumerate );
      bOk &= parse<std::string>( sFileName, vm, sValue_Nut_Host, values.nut.sHost );
      bOk &= parse<std::string>( sFileName, vm, sValue_Nut_UserName, values.nut.sUserName );
      bOk &= parse<std::string>( sFileName, vm, sValue_Nut_Password, values.nut.sPassword );
      bOk &= parse<size_t>( sFileName, vm, sValue_Nut_PollInterval, values.nut.nPollInterval );

      if ( 0 < vm.count( sValue_Nut_Field ) ) { // defauilt to publish all fields
        vField = vm[ sValue_Nut_Field ].as<vName_t>();
      }
      if ( 0 < vm.count( sValue_Nut_Numeric ) ) { // default to publish all fields as text
        vNumeric = vm[ sValue_Nut_Numeric ].as<vName_t>();
      }

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

  values.nut.setField.clear();
  for ( const auto& name: vField ) {
    values.nut.setField.insert( name );
  }

  values.nut.setNumeric.clear();
  for ( const auto& name: vNumeric ) {
    values.nut.setNumeric.insert( name );
  }

  return bOk;
}

} // namespace config
