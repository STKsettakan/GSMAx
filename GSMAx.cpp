#include "GSMAx.h"
#include <SoftwareSerial.h>
GSMAx gsm;

GSMAx :: GSMAx()
{}

void GSMAx :: power()
{
	digitalWrite(_pinpwr,LOW);
	digitalWrite(_pinpwr,HIGH);
	delay(2000);
	digitalWrite(_pinpwr,LOW);
}
void GSMAx :: reset()
{
	digitalWrite(_pinrst,HIGH);
	digitalWrite(_pinrst,LOW);
	delay(1000);
	digitalWrite(_pinrst,HIGH);
	
}
void GSMAx :: begin(SoftwareSerial *serial,long baud,unsigned char pinPWR)
{
	_pinpwr = pinPWR;
	pinMode(_pinpwr,OUTPUT);
	serial->begin(baud);
	_Serial = serial;	
}
void GSMAx :: begin(HardwareSerial *serial,long baud,unsigned char pinPWR)
{
	_pinpwr = pinPWR;
	pinMode(_pinpwr,OUTPUT);
	serial->begin(baud);
	_Serial = serial;
}
bool GSMAx :: waitReady()
{
	_Serial->println(F("AT"));
	GSMAx_RES res = wait_rx_bc(500,"OK");
	return(res.status);
}
bool GSMAx :: closeEcho()
{
	_Serial->println(F("ATE0"));
	GSMAx_RES res = wait_rx_bc(500,"OK");
	return(res.status);
}
unsigned char GSMAx :: getSignal()
{
	int sig;
	_Serial->println(F("AT+CSQ"));
	GSMAx_RES res = wait_rx_bc(3000,"+CSQ");
	if(res.status)
	{
		int index = res.data.indexOf(F(":"));
		int index2 = res.data.indexOf(F(","));
		sig = res.data.substring(index+2,index2).toInt();
		res = wait_rx_bc(1000,"OK");
	}
	else
		return(99);
	return(sig);
}
bool GSMAx :: connectNet(String apn)
{
	String data = "at+cgdcont=1,\"IP\",\""+apn+"\"";
	_Serial->println(data);
	GSMAx_RES res = wait_rx_bc(3000,"OK");
	if(res.status)
	{
		_Serial->println("at+cgact=1,1");
		res = wait_rx_bc(3000,"OK");
		return(res.status);
	}
	else
	{
		return(res.status);
	}
}
bool GSMAx :: connectServer(String type,String url,String port)
{
	String data = "at+cipstart=\""+type+"\",\""+url+"\","+port;
	_Serial->println(data);
	GSMAx_RES res = wait_rx_bc(5000,"OK");
	if(res.temp.indexOf("CONNECT OK")!=-1)
	{
		res = wait_rx_bc(1000,"OK");
		return(true);
	}
	return(false);
	
}
bool GSMAx :: connectServer(String url,String port)
{
	return(connectServer("TCP",url,port));
}
bool GSMAx :: closeServer()
{
	_Serial->println(F("AT+CIPCLOSE"));
	GSMAx_RES res = wait_rx_bc(5000,"OK");
	return(res.status);
}

bool GSMAx :: startSend()
{
	_Serial->println(F("AT+CIPSEND"));
	GSMAx_RES res = wait_rx_bc(5000,">");
	return(res.status);
}
bool GSMAx :: startSend(unsigned char length)
{
	_Serial->print(F("AT+CIPSEND="));
	_Serial->println(String(length));
	GSMAx_RES res = wait_rx_bc(5000,">");
	return(res.status);
}
bool GSMAx :: stopSend()
{
	_Serial->write(0x1A);
	GSMAx_RES res = wait_rx_bc(5000,"OK");
	return(res.status);
}

GSMAx_RES GSMAx:: wait_rx_bc(long tout,String str_wait)
{
	unsigned long pv_ok = millis();
	unsigned long current_ok = millis();
	String input;
	unsigned char flag_out=1;
	unsigned char res=-1;
	GSMAx_RES res_;
	res_.temp="";
	while(flag_out)
	{
		if(_Serial->available())
		{
			input = _Serial->readStringUntil('\n');
			//Serial.println(input);
			res_.temp+=input;
			if(input.indexOf(str_wait)!=-1)
			{
				res=1;
				flag_out=0;
			}	
		    else if(input.indexOf("ERROR")!=-1)
			{
				res=0;
				flag_out=0;
			}
		}
		current_ok = millis();
		if (current_ok - pv_ok>= tout) 
		{
			flag_out=0;
			res=0;
			pv_ok = current_ok;
		}
	}
	
	res_.status = res;
	res_.data = input;
	return(res_);
}

/* Code Serial */
String GSMAx ::  readStringUntil(char data)
{
	 return _Serial->readStringUntil(data);
}
size_t GSMAx ::  print(String data)
{
     return _Serial->print(data);
}
size_t GSMAx ::  println(String data)
{
     return _Serial->println(data);
}
void GSMAx ::  print(unsigned char data,int type)
{
	_Serial->print(data,type);
}
void GSMAx ::  print(int data,int type)
{
	_Serial->print(data,type);
}
void GSMAx ::  print(unsigned int data,int type)
{
	_Serial->print(data,type);
}
void GSMAx ::  print(long data,int type)
{
	_Serial->print(data,type);
}

size_t GSMAx ::  print(String data,int type)
{
   int i=0;
   while(data[i])
   {
		_Serial->print(data[i],type);
		i++;   
   }
   return(i-1);
   
   // Serial.print(data,type);
	// return _Serial->print(data,type);
}
size_t GSMAx ::  println(String data,int type)
{
	int i=0;
	while(data[i])
	{
		_Serial->print(data[i],type);
		i++;   
	}
	_Serial->println("");
    return(i+1);


	//return _Serial->println(data,type);
}
int GSMAx ::  peek()
{
     return _Serial->peek();
}
size_t GSMAx ::  write(uint8_t byte)
{
     return _Serial->write(byte);
}
int GSMAx ::  read()
{
     return _Serial->read();
}
int GSMAx ::  available()
{
     return _Serial->available();
}
void GSMAx ::  flush()
{
     _Serial->flush();
}
