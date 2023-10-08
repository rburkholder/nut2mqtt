/*
  project: nut2mqtt
  file:    mqtt.hpp
  author:  raymond@burkholder.net
  date:    2023/10/01 15:58:25
*/

#include <mutex>
#include <string>
#include <stdexcept>
#include <functional>
#include <unordered_map>

#include <boost/asio/io_context.hpp>

#include "Config.hpp"

#include <MQTTClient.h>

// NOTE: config::Values needs to be long-lived

namespace asio = boost::asio; // from <boost/asio/context.hpp>

class Mqtt {
public:

  struct runtime_error: std::runtime_error {
    int rc;
    runtime_error( const std::string& e, int rc_ )
    : std::runtime_error( e ) {}
  };

  Mqtt( const config::Values&, boost::asio::io_context&, const char* szHostName );
  ~Mqtt();

  using fPublishComplete_t = std::function<void(bool,int)>;
  void Publish( const std::string& sTopic, const std::string& sMessage, fPublishComplete_t&& );

protected:
private:

  asio::io_context& m_io_context;

  bool m_bCreated;
  bool m_bConnected;
  MQTTClient m_clientMqtt;
  MQTTClient_connectOptions m_conn_opts;
  MQTTClient_message m_pubmsg;
  const std::string m_sMqttUrl;

  std::mutex m_mutexDeliveryToken;

  using umapDeliveryToken_t = std::unordered_map<MQTTClient_deliveryToken, fPublishComplete_t>;
  umapDeliveryToken_t m_umapDeliveryToken;

  static int messageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* message);
  static void deliveryComplete(void* context, MQTTClient_deliveryToken dt);
  static void connectionLost(void* context, char* cause);

};
