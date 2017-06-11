#ifndef GSMAx_h
#define GSMAx_h

#include <Arduino.h>
#include <Stream.h>
#include <SoftwareSerial.h>

struct GSMAx_RES
{
	char status;
	String data;
	String temp;
};




class GSMAx 
{
public:
	GSMAx();
	
	void begin(SoftwareSerial *serial,long baud,unsigned char pinPWR);	
	void begin(HardwareSerial *serial,long baud,unsigned char pinPWR);
	void power();
	void reset();
	bool waitReady();
	bool closeEcho();
	unsigned char getSignal();
	bool connectNet(String apn);
	bool connectServer(String type,String url,String port);
	bool connectServer(String url,String port);
	bool closeServer();
	bool startSend();
	bool startSend(unsigned char length);
	bool stopSend();
	
	
	
	GSMAx_RES wait_rx_bc(long tout,String str_wait);
	int  peek();
	virtual size_t write(uint8_t byte);
	int read();
    int available();
    virtual void flush();
	void print(unsigned char data,int type);
	void print(int data,int type);
	void print(unsigned int data,int type);
	void print(long data,int type);
	size_t print(String data);
	size_t println (String data);
	size_t print(String data,int type);
	size_t println (String data,int type);
	String readStringUntil(char data);
	
	
protected:
	Stream *_Serial;
	unsigned char _pinpwr;
	unsigned char _pinrst;
};

extern GSMAx gsm;














#endif





























