/*
  project: nut2mqtt
  file:    mqtt.hpp
  author:  raymond@burkholder.net
  date:    2023/10/01 15:58:25
*/

#include <stdexcept>
#include <string>

#include "Config.hpp"

#include <MQTTClient.h>

class Mqtt {
public:

  struct runtime_error: std::runtime_error {
    int rc;
    runtime_error( const std::string& e, int rc_ )
    : std::runtime_error( e ) {}
  };

  Mqtt( const config::Values&, const char* szHostName );
  ~Mqtt();

  void Publish( const std::string& sTopic, const std::string& sMessage );
protected:
private:
  bool m_bCreated;
  bool m_bConnected;
  MQTTClient m_clientMqtt;
  MQTTClient_connectOptions m_conn_opts;
  MQTTClient_message m_pubmsg;
  MQTTClient_deliveryToken m_token;
  const std::string m_sMqttUrl;
};
