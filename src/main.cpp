/*
  project: nut2mqtt
  author:  raymond@burkholder.net
  date:    2023/09/14 20:30:00
*/

#include <unistd.h>

#include <limits.h>

#include <memory>
#include <iostream>

#include <boost/lexical_cast.hpp>

#include <boost/asio/post.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include <nutclient.h>

#include "Config.hpp"
#include "mqtt.hpp"

// TODO:
//   accept 'set' for writes
//   track changes and emit delta
//   accept 'get' for refresh, specific key for single value

namespace asio = boost::asio; // from <boost/asio/context.hpp>

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

  boost::asio::io_context m_io_context;

  using work_guard_t = asio::executor_work_guard<boost::asio::io_context::executor_type>;
  using pWorkGuard_t = std::unique_ptr<work_guard_t>;

  pWorkGuard_t m_pWorkGuard;

  m_pWorkGuard = std::make_unique<work_guard_t>( asio::make_work_guard( m_io_context ) );

  asio::steady_timer timerPollInterval( m_io_context );

  using setName_t = std::set<std::string>;
  using vValue_t = std::vector<std::string>;

  nut::Client *clientNut( nullptr );

  setName_t setDeviceNames;

  try {
    clientNut = new nut::TcpClient( choices.nut.sHost, 3493 );
    setDeviceNames = clientNut->getDeviceNames();
  }
  catch( const std::logic_error& e ) {
    std::cerr << "client open problems: " << e.what() << std::endl;
    return( EXIT_FAILURE );
  }

  // https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/signal_set.html
  boost::asio::signal_set signals( m_io_context, SIGINT ); // SIGINT is called '^C'
  //signals.add( SIGKILL ); // not allowed here
  signals.add( SIGHUP ); // use this as a config change?
  //signals.add( SIGINFO ); // control T - doesn't exist on linux
  signals.add( SIGTERM );
  signals.add( SIGQUIT );
  signals.add( SIGABRT );

  using fSignals_t = std::function<void(const boost::system::error_code&, int)>;
  fSignals_t fSignals =
    [&m_pWorkGuard,&signals,&fSignals,&timerPollInterval](const boost::system::error_code& error_code, int signal_number){
      std::cout
        << "signal"
        << "(" << error_code.category().name()
        << "," << error_code.value()
        << "," << signal_number
        << "): "
        << error_code.message()
        << std::endl;

      bool bContinue( true );

      switch ( signal_number ) {
        case SIGHUP:
          std::cout << "sig hup noop" << std::endl;
          break;
        case SIGTERM:
          std::cout << "sig term" << std::endl;
          bContinue = false;
          break;
        case SIGQUIT:
          std::cout << "sig quit" << std::endl;
          bContinue = false;
          break;
        case SIGABRT:
          std::cout << "sig abort" << std::endl;
          bContinue = false;
          break;
        case SIGINT:
          std::cout << "sig int" << std::endl;
          bContinue = false;
          break;
        default:
          break;
      }

      if ( bContinue ) {
        signals.async_wait( fSignals );
      }
      else {
        m_pWorkGuard.reset();
        timerPollInterval.cancel(); // comes after the reset
        bContinue = false;
      }
    };

  signals.async_wait( fSignals );

  using fPoll_t = std::function<void( bool, bool )>;
  fPoll_t fPoll =
    [&m_pWorkGuard, &setDeviceNames, &clientNut, &choices, &pMqtt, &timerPollInterval, &fPoll]( bool bAll, bool bEnumerate ){

      setName_t setVariable_RO;
      setName_t setVariable_RW;
      setName_t setCommand;

      for ( const auto& sDeviceName: setDeviceNames ) {
        if ( bEnumerate ) {
          std::cout << "\nDevice: " << sDeviceName << std::endl;
        }

        try {
          nut::Device mydev = clientNut->getDevice( sDeviceName );

          setVariable_RO = mydev.getVariableNames();
          if ( bEnumerate ) {
            std::cout << "  Variables (read-only):" << std::endl;
            for ( const auto& sName: setVariable_RO ) {
              std::cout << "    " << sName << ":";
              vValue_t vValue = mydev.getVariableValue( sName );
              for ( const auto& sValue: vValue ) {
                std::cout << " " << sValue;
              }
              std::cout << std::endl;
            }
          }

          std::string sMessage;
          bool bComma( false );
          setName_t setVariable;
          if ( bAll ) setVariable = setVariable_RO;
          else {
            if ( choices.nut.setField.empty() ) {
              setVariable = setVariable_RO;
            }
            else {
              setVariable = choices.nut.setField;
            }
          }

          for ( const auto& sName: setVariable ) {
            setName_t::const_iterator iterRO = setVariable_RO.find( sName );
            if ( setVariable_RO.end() != iterRO ) {
              vValue_t vValue = mydev.getVariableValue( sName ); // empty set if does not exist?
              for ( const std::string& sValue: vValue ) {
                if ( 1 == vValue.size() ) {
                  if ( bComma ) {
                    sMessage += ',';
                  }
                  else bComma = true;
                  sMessage += '"' + sName + '"' + ':';

                  bool bNumeric( true );
                  setName_t::const_iterator iterNumeric = choices.nut.setNumeric.find( sName );
                  if ( choices.nut.setNumeric.end() == iterNumeric ) {
                    bNumeric = false;
                  }
                  if ( bNumeric ) {
                    try {
                      double value = boost::lexical_cast<double>( sValue );
                    }
                    catch( const boost::bad_lexical_cast& e ) {
                      bNumeric = false;
                    }
                  }
                  if ( bNumeric ) {
                    sMessage += sValue;
                  }
                  else {
                    sMessage += '"' + sValue + '"';
                  }
                }
              }
            }
          }

          const std::string sTopic = choices.mqtt.sTopic + '/' + sDeviceName;
          sMessage = '{' + sMessage + '}';

          //std::cout << "topic: " << sTopic << std::endl;
          //std::cout << sMessage << std::endl;

          try {
            pMqtt->Publish( sTopic, sMessage );
          }
          catch( const Mqtt::runtime_error& e ) {
            std::cerr << "mqtt error: " << e.what() << '(' << e.rc << ')' << std::endl;
          }

          setVariable_RW = mydev.getRWVariableNames();
          if ( bEnumerate ) {
            std::cout << "\n  Variables (read-write):" << std::endl;

            for ( const auto& sName: setVariable_RW ) {
              std::cout << "    " << sName << ":";
              vValue_t vValue = mydev.getVariableValue( sName );
              for ( const std::string& sValue: vValue ) {
                std::cout << " " << sValue;
              }
              std::cout << std::endl;
            }
          }

          setCommand = mydev.getCommandNames();
          if ( bEnumerate ) {
            std::cout << std::endl;
            std::cout << "\n  Commands:" << std::endl;
            for ( const auto& sName: setCommand ) {
              std::cout << "    " << sName;
              std::cout << std::endl;
            }
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
        catch( ... ) {
          std::cerr << "unknown problem: " << std::endl;
        }

      }

      if ( nullptr != m_pWorkGuard.get() ) {
        timerPollInterval.expires_after( std::chrono::seconds( choices.nut.nPollInterval ) );
        timerPollInterval.async_wait( [&fPoll]( const boost::system::error_code& e ){
          if ( 0 == e.value() ) {
            fPoll( false, false );
          }
          else {
            // Operation canceled [system:125]
            std::cerr << "timer msg: " << e.what() << std::endl;
          }
        } );
      }
    }; // fPoll

  std::cout << "ctrl-c to end" << std::endl;

  asio::post( m_io_context, std::bind( fPoll, true, choices.nut.bEnumerate ) );

  m_io_context.run();

  if ( nullptr != clientNut ) {
    delete clientNut;
    clientNut = nullptr;
  }

  return EXIT_SUCCESS;
}
