
#include <iostream>

#include <nutclient.h> // requires 'sudo apt install libnutclient-dev'

int main( int argc, char **argv ) {

  std::cout << "Nut2MQTT (c)2023 One Unified Net Limited" << std::endl;

  using setName_t = std::set<std::string>;
  using vValue_t = std::vector<std::string>;

  setName_t setVariable_RO;
  setName_t setVariable_RW;
  setName_t setCommand;

  nut::Client *client( nullptr );

  try {

    client = new nut::TcpClient( std::string( "localhost" ), 3493 );

    //nut::Device mydev = client->getDevice(argv[1]);
    nut::Device mydev = client->getDevice( "nuc-sm1500" );
    std::cout << "Description: " << mydev.getDescription() << std::endl;

    std::cout << std::endl;

    std::cout << "Variables (read-only)" << std::endl;
    setVariable_RO = mydev.getVariableNames();
    for ( const auto& sName: setVariable_RO ) {
      std::cout << sName << ":";
      vValue_t vValue = mydev.getVariableValue( sName );
      for ( const auto& sValue: vValue ) {
        std::cout << " " << sValue;
      }
      std::cout << std::endl;
    }

    std::cout << std::endl;

    std::cout << "Variables (read-write)" << std::endl;
    setVariable_RW = mydev.getRWVariableNames();
    for ( const auto& sName: setVariable_RW ) {
      std::cout << sName << ":";
      vValue_t vValue = mydev.getVariableValue( sName );
      for ( const auto& sValue: vValue ) {
        std::cout << " " << sValue;
      }
      std::cout << std::endl;
    }

    std::cout << std::endl;

    std::cout << "Commands" << std::endl;
    setCommand = mydev.getCommandNames();
    for ( const auto& sName: setCommand ) {
      std::cout << sName;
      std::cout << std::endl;
    }

  }
  catch ( const std::logic_error& e ) {
    std::cout << "open problems: " << e.what() << std::endl;
  }

  if ( nullptr != client ) {
    delete client;
  }

  return 0;
}

