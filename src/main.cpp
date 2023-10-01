/*
  project: nut2mqtt
  author:  raymond@burkholder.net
  date:    2023/09/14 20:30:00
*/

#include <unistd.h>

#include <limits.h>

#include <memory>
#include <iostream>

#include <nutclient.h>

#include "Config.hpp"
#include "mqtt.hpp"

// TODO:
//   poll on interval
//   accept 'set' for writes
//   track changes and emit delta
//   accetp 'get' for refresh, specific key for single value

int main( int argc, char **argv ) {

  static const std::string c_sConfigFilename( "nut2mqtt.cfg" );

  std::cout << "Nut2MQTT (c)2023 One Unified Net Limited" << std::endl;

  config::Values choices;

  if ( Load( c_sConfigFilename, choices ) ) {
  }
  else {
    return EXIT_FAILURE;
  }

  int rc;
  char szHostName[ HOST_NAME_MAX + 1 ];
  rc = gethostname( szHostName, HOST_NAME_MAX + 1 );
  if ( 0 != rc ) {
    std::cerr << "failure with gethostname" << std::endl;
    return( EXIT_FAILURE );
  }

  std::unique_ptr<Mqtt> pMqtt;
  try {
    pMqtt = std::make_unique<Mqtt>( choices, szHostName );
  }
  catch ( const Mqtt::runtime_error& e ) {
    std::cerr << "mqtt error: " << e.what() << '(' << e.rc << ')' << std::endl;
    return( EXIT_FAILURE );
  }

  using setName_t = std::set<std::string>;
  using vValue_t = std::vector<std::string>;

  setName_t setVariable_RO;
  setName_t setVariable_RW;
  setName_t setCommand;

  nut::Client *clientNut( nullptr );

  try {

    clientNut = new nut::TcpClient( choices.nut.sHost, 3493 );

    setName_t setDeviceNames = clientNut->getDeviceNames();

    for ( const auto& sDeviceName: setDeviceNames ) {
      std::cout << "Device: " << sDeviceName << std::endl;

      try {
        nut::Device mydev = clientNut->getDevice( sDeviceName );

        std::cout << "Variables (read-only):" << std::endl;
        setVariable_RO = mydev.getVariableNames();
        for ( const auto& sName: setVariable_RO ) {
          std::cout << "  " << sName << ":";
          vValue_t vValue = mydev.getVariableValue( sName );
          for ( const auto& sValue: vValue ) {
            std::cout << " " << sValue;
          }
          std::cout << std::endl;
        }

        std::cout << std::endl;

        std::cout << "Variables (read-write):" << std::endl;

        std::string sMessage;
        bool bComma( false );

        setVariable_RW = mydev.getRWVariableNames();
        for ( const auto& sName: setVariable_RW ) {
          std::cout << "  " <<sName << ":";
          vValue_t vValue = mydev.getVariableValue( sName );
          for ( const std::string& sValue: vValue ) {
            std::cout << " " << sValue;
            if ( 1 == vValue.size() ) {
              if ( bComma ) {
                sMessage += ',';
              }
              else bComma = true;
              sMessage += '"' + sName + '"' + ':' + '"' + sValue + '"';
            }
          }
          std::cout << std::endl;
        }

        const std::string sTopic = choices.mqtt.sTopic + '/' + sDeviceName;
        sMessage = '{' + sMessage + '}';

        std::cout << "topic: " << sTopic << std::endl;
        std::cout << sMessage << std::endl;

        try {
          pMqtt->Publish( sTopic, sMessage );
        }
        catch( const Mqtt::runtime_error& e ) {
          std::cerr << "mqtt error: " << e.what() << '(' << e.rc << ')' << std::endl;
        }

        std::cout << std::endl;

        std::cout << "Commands:" << std::endl;
        setCommand = mydev.getCommandNames();
        for ( const auto& sName: setCommand ) {
          std::cout << "  " << sName;
          std::cout << std::endl;
        }

      }
      catch( const std::logic_error& e ) {
        std::cerr << "open problems: " << e.what() << std::endl;
      }
      catch( const nut::IOException& e ) {
        std::cerr << "nut io problem: " << e.str() << std::endl;
      }
      catch( nut::NutException& e ) {
        std::cerr << "nut problem: " << e.str() << std::endl;
      }

      std::cout << std::endl;
    }

  }
  catch( const std::logic_error& e ) {
    std::cerr << "client open problems: " << e.what() << std::endl;
  }

  if ( nullptr != clientNut ) {
    delete clientNut;
    clientNut = nullptr;
  }

  //sleep( 20 );

  return EXIT_SUCCESS;
}
