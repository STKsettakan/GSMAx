#ifndef GSMAx_mqtt_h
#define GSMAx_mqtt_h
#include "GSMAx.h"


#define MQTT_MAX_PACKET_SIZE 128

class GSMAxMQTT 
{
public:
	GSMAxMQTT(); 
	bool connectMQTTServer(String web , String port);
	bool disconnectMQTTServer();
	unsigned char login(String id,String user,String pass); 
	void publish(char *topic ,int lentopic, char *payload ,int lenpay);
	void publish(String topic , String payload);
	void publish(String topic , int data);
	void publish(String topic , float data,unsigned char point=2);
	
	void subscribe(char *topic,int topiclen);
	void subscribe(String topic);
	void Ping();
	void loop();
	void (*callback)(String topic ,char *playload,unsigned char length);
	bool connectState();
	private:
	uint8_t buffer[MQTT_MAX_PACKET_SIZE];
	bool write_tcp(uint8_t* buf, uint8_t length);
	void clear_buffer();
};






















#endif