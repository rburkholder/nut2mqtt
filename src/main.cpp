/*
  project: nut2mqtt
  author:  raymond@burkholder.net
  date:    2023/09/14 20:30:00
*/

#include <iostream>

#include <boost/asio/io_context.hpp>

#include "Loop.hpp"
#include "Config.hpp"

namespace asio = boost::asio;

int main( int argc, char **argv ) {

  static const std::string c_sConfigFilename( "nut2mqtt.cfg" );

  std::cout << "Nut2MQTT (c)2023 One Unified Net Limited" << std::endl;

  config::Values choices;

  if ( Load( c_sConfigFilename, choices ) ) {
  }
  else {
    return EXIT_FAILURE;
  }

  asio::io_context io_context;

  try {
    Loop loop( choices, io_context );
    io_context.run();
  }
  catch(...) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
