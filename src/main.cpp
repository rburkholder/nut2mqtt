
#include <iostream>

// requires apt install libnutclient-dev
#include <nutclient.h>

int main( int argc, char **argv ) {
  std::cout << "hello" << std::endl;

  nut::Client *client;
  client = new nut::TcpClient("localhost", 3493);

  nut::Device mydev = client->getDevice(argv[1]);
  std::cout << "Description: " << mydev.getDescription() << std::endl;
  nut::Variable var = mydev.getVariable("device.model");
  std::cout << "Model: " <<  var.getValue()[0] << std::endl;

  delete client;

  return 0;
}

