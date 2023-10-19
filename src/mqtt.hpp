/*
  project: nut2mqtt
  file:    mqtt.hpp
  author:  raymond@burkholder.net
  date:    2023/10/01 15:58:25
*/

#include <mutex>
#include <string>
#include <thread>
#include <stdexcept>
#include <functional>
#include <unordered_map>

#include "Config.hpp"

#include <MQTTClient.h>

// NOTE: config::Values needs to be long-lived

class Mqtt {
public:

  struct runtime_error: std::runtime_error {
    int rc;
    runtime_error( const std::string& e, int rc_ )
    : std::runtime_error( e ) {}
  };

  Mqtt( const config::Values&, const char* szHostName );
  ~Mqtt();

  using fPublishComplete_t = std::function<void(bool,int)>;
  void Publish( const std::string& sTopic, const std::string& sMessage, fPublishComplete_t&& );

protected:
private:

  enum class EState{ init, created, connecting, connected, start_reconnect, retry_connect, disconnecting, destruct } m_state;

  std::thread m_threadConnect;

  MQTTClient m_clientMqtt;
  MQTTClient_connectOptions m_conn_opts;
  MQTTClient_message m_pubmsg;
  const std::string m_sMqttUrl;

  std::mutex m_mutexDeliveryToken;

  using umapDeliveryToken_t = std::unordered_map<MQTTClient_deliveryToken, fPublishComplete_t>;
  umapDeliveryToken_t m_umapDeliveryToken;

  static int MessageArrived( void* context, char* topicName, int topicLen, MQTTClient_message* message );
  static void DeliveryComplete( void* context, MQTTClient_deliveryToken token );
  static void ConnectionLost( void* context, char* cause );

  void Connect();

};
