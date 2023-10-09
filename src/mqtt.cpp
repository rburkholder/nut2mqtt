/*
  project: nut2mqtt
  file:    mqtt.cpp
  author:  raymond@burkholder.net
  date:    2023/10/01 15:58:25
*/

#include <chrono>
#include <cassert>
#include <iostream>

#include "mqtt.hpp"

namespace {
  unsigned int c_nQOS( 1 );
  unsigned int c_nTimeOut( 2 ); // seconds
}

Mqtt::Mqtt( const config::Values& choices, const char* szHostName )
: m_state( EState::init )
, m_conn_opts( MQTTClient_connectOptions_initializer )
, m_pubmsg( MQTTClient_message_initializer )
, m_sMqttUrl( "tcp://" + choices.mqtt.sHost + ":1883" )
{
  m_conn_opts.keepAliveInterval = 20;
  m_conn_opts.cleansession = 1;
  m_conn_opts.connectTimeout = c_nTimeOut;
  m_conn_opts.username = choices.mqtt.sUserName.c_str();
  m_conn_opts.password = choices.mqtt.sPassword.c_str();

  int rc;

  rc = MQTTClient_create(
    &m_clientMqtt, m_sMqttUrl.c_str(), szHostName, // might use choices.mqtt.id
    MQTTCLIENT_PERSISTENCE_NONE, nullptr
    );

  if ( MQTTCLIENT_SUCCESS != rc ) {
    throw( runtime_error( "Failed to create client", rc ) );
  }

  m_state = EState::created;

  rc = MQTTClient_setCallbacks( m_clientMqtt, this, &Mqtt::ConnectionLost, &Mqtt::MessageArrived, &Mqtt::DeliveryComplete );

  rc = MQTTClient_connect( m_clientMqtt, &m_conn_opts );
  if ( MQTTCLIENT_SUCCESS == rc ) {
    m_state = EState::connected;
  }
  else {
    m_state = EState::connecting;
    Connect();
  }

}

Mqtt::~Mqtt() {
  int rc {};
  switch ( m_state ) {
    case EState::retry_connect:
      m_state = EState::disconnecting;
      if ( m_threadConnect.joinable() ) m_threadConnect.join();
      // fall through to EState::connected:
    case EState::connected:
      m_state = EState::disconnecting;
      rc = MQTTClient_disconnect( m_clientMqtt, 1000 );
      if ( MQTTCLIENT_SUCCESS != rc ) {
        std::cerr << "Failed to disconnect, return code " << rc << std::endl;
      }
      m_state = EState::destruct;
      break;
    default:
      break;
  }

  if ( EState::init != m_state ) {
    assert( EState::destruct == m_state );
    MQTTClient_destroy( &m_clientMqtt );
  }
}

void Mqtt::Connect() {
  assert( EState::init != m_state );
  if ( 1 == MQTTClient_isConnected( m_clientMqtt ) ) {
    assert( EState::connected == m_state );
    std::cerr << "mqtt is already connected" << std::endl;
  }
  else {
    m_state = EState::retry_connect;
    m_threadConnect = std::move( std::thread(
      [this](){
        while ( EState::retry_connect == m_state ) {
          int rc = MQTTClient_connect( m_clientMqtt, &m_conn_opts );
          if ( MQTTCLIENT_SUCCESS == rc ) {
            m_state = EState::connected;
          }
          else {
            std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
          }
        }
      } ) );
  }
}

void Mqtt::Publish( const std::string& sTopic, const std::string& sMessage, fPublishComplete_t&& fPublishComplete ) {
  if ( EState::connected == m_state ) {
    m_pubmsg.payload = (void*) sMessage.begin().base();
    m_pubmsg.payloadlen = sMessage.size();
    m_pubmsg.qos = c_nQOS;
    m_pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    int rc = MQTTClient_publishMessage( m_clientMqtt, sTopic.c_str(), &m_pubmsg, &token );
    if ( MQTTCLIENT_SUCCESS != rc ) {
      fPublishComplete( false, rc );
      //throw( runtime_error( "Failed to publish message", rc ) );
    }
    else {
      std::lock_guard<std::mutex> lock( m_mutexDeliveryToken );
      umapDeliveryToken_t::iterator iterDeliveryToken = m_umapDeliveryToken.find( token );
      if ( m_umapDeliveryToken.end() == iterDeliveryToken ) {
        m_umapDeliveryToken.emplace( token, std::move( fPublishComplete ) );
      }
      else {
        std::cerr << "delivery token " << token << " already delivered" << std::endl;
        fPublishComplete( true, 0 );
        assert( nullptr == iterDeliveryToken->second );
        m_umapDeliveryToken.erase( iterDeliveryToken );
      }
    }
  }
}

void Mqtt::ConnectionLost( void* context, char* cause ) {
  assert( context );
  Mqtt* self = reinterpret_cast<Mqtt*>( context );
  std::cerr << "mqtt connection lost, reconnecting ..." << std::endl;
  assert( EState::connected == self->m_state );
  self->Connect();
}

int Mqtt::MessageArrived( void* context, char* topicName, int topicLen, MQTTClient_message* message ) {
  assert( context );
  Mqtt* self = reinterpret_cast<Mqtt*>( context );
  assert( 0 == topicLen  );
  std::cout << "mqtt message: " << std::string( topicName, topicLen ) << " " << std::string( (const char*) message->payload, message->payloadlen ) << std::endl;
	//const std::string_view svTopic( topicName );
	//const std::string_view svMessage( (char*)message->payload, message->payloadlen );
  MQTTClient_freeMessage( &message );
  MQTTClient_free( topicName );
  return 1;
}

void Mqtt::DeliveryComplete( void* context, MQTTClient_deliveryToken token ) {
	// not called with QoS0
  assert( context );
  Mqtt* self = reinterpret_cast<Mqtt*>( context );
  //std::cout << "mqtt delivery complete" << std::endl;
  std::lock_guard<std::mutex> lock( self->m_mutexDeliveryToken );
  umapDeliveryToken_t::iterator iterDeliveryToken = self->m_umapDeliveryToken.find( token );
  if ( self->m_umapDeliveryToken.end() == iterDeliveryToken ) {
    std::cerr << "delivery token " << token << " not yet registered" << std::endl;
    self->m_umapDeliveryToken.emplace( token, nullptr );
  }
  else {
    iterDeliveryToken->second( true, 0 );
    self->m_umapDeliveryToken.erase( iterDeliveryToken );
  }
}
