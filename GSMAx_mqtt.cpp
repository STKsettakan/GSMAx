#include "GSMAx.h"
#include "GSMAx_mqtt.h"

unsigned long pv_ping = 0;  
const long interval_ping = 10000; 

bool connected = false;
void func_null(String topic ,char *playload,unsigned char length){}
GSMAxMQTT ::GSMAxMQTT() 
{
	callback = func_null;
}
bool GSMAxMQTT :: connectMQTTServer(String web , String port)
{
	return(gsm.connectServer(web,port));
}
bool GSMAxMQTT :: disconnectMQTTServer()
{
	return(gsm.closeServer());
}
unsigned char GSMAxMQTT :: login(String id,String user,String pass) 
{
	connected = false;
		    // Leave room in the buffer for header and variable length field
            uint16_t length = 0;
            unsigned int j;
			unsigned char ctrl_flag = 0x02;
			buffer[0]=0x10;    // Control packet type     (Fix=10)
			buffer[1]=0x00;    // Remain Length
			buffer[2]=0x00;    // Protocol name Lengh MSB (Fix=0)
			buffer[3]=0x04;    // Protocol name Lengh LSB (Fix=4)
			buffer[4]='M';     // Protocol name   (Fix=M)
			buffer[5]='Q';     // Protocol name   (Fix=Q)
			buffer[6]='T';     // Protocol name   (Fix=T)
			buffer[7]='T';     // Protocol name   (Fix=T)
			buffer[8]=0x04;    // Protocol Level  (Fix=4)
			//buffer[9]=0xC2;    // Control Flag (Bit7 : user name flag),(Bit6 : password flag) , (Bit5 : will retrain) , (Bit4-3 : will Qos) , (Bit2 : will flag) , (Bit1 : clear section) , (Bit0 : fix0)
			//buffer[9]=0x02;
			buffer[10]=0x00;   // keep alive MSB
			buffer[11]=0x0F;   // keep alive LSB time ping 16 bit (seconds)

			
			unsigned char  i;
			unsigned char len = id.length();
			buffer[12] = (len>>8)&0x00FF;   // id Lengh MSB
			buffer[13] = len&0x00FF;        // id Lengh LSB
			
			length = 14;
			for(i=0;i<len;i++)
			{
				buffer[length] = id[i];
				length++;
			}

			len = user.length();
			if(len>0)
				ctrl_flag |= 1<<7;
			buffer[length] = (len>>8)&0x00FF;   // id Lengh MSB
			length++;
			buffer[length] = len&0x00FF;        // id Lengh LSB
			length++;
			for(i=0;i<len;i++)
			{
				buffer[length] = user[i];
				length++;
			}

			len = pass.length();
			if(len>0)
				ctrl_flag |= 1<<6;
			buffer[length] = (len>>8)&0x00FF;   // id Lengh MSB
			length++;
			buffer[length] = len&0x00FF;        // id Lengh LSB
			length++;
			for(i=0;i<len;i++)
			{
				buffer[length] = pass[i];
				length++;
			}
			buffer[1]= length-2;
			buffer[9]=ctrl_flag;
			
			if(write_tcp(buffer,length))
			{
				GSMAx_RES res = gsm.wait_rx_bc(3000,"+CIPRCV");
				if(res.status)
				{
					int index = res.data.indexOf(":");
					int index2 = res.data.indexOf(",");
					int len = res.data.substring(index+1,index2).toInt();
					//Serial.println("LEN = "+String(len));
					index2=index2+1;
					//for(unsigned char i=0;i<len;i++)
					//{
						//Serial.print(res.data[index2+i],HEX);
						//Serial.print(" ");
					//}
					return(1);
					
				}
			}
			return(0);
			

}

//+CIPRCV:4,20 02 00 00 0D0A0D0A 00

bool GSMAxMQTT :: write_tcp(uint8_t* buf, uint8_t length)
{
	
	//Serial.println("len = ");
	//Serial.println(length);
	if(gsm.startSend(length))
	{
		for(unsigned char i=0;i<length;i++)
		{
			gsm.write(buf[i]);
			//Serial.print(" ");
			//if(buf[i]<=0x0F)
			//Serial.print("0");
			//Serial.print(buf[i],HEX);			
		}
		
		GSMAx_RES res = gsm.wait_rx_bc(500,"OK");
		return(res.status);
		
	}
	else
		return(false);
} 

void GSMAxMQTT :: publish(char *topic ,int lentopic, char *payload ,int lenpay)
{
	 
	 clear_buffer();

	 buffer[0]=0x30;    // Control packet type (Fix=3x) ,x bit3 = DUP , xbit2-1 = Qos level , xbit0 = Retain
	 buffer[1]=0x00;     // remaining length

	 int i=0;
	 int len = lentopic;
	 buffer[2]=(len>>8)&0x00FF;    //  topic Lengh MSB
	 buffer[3]=len&0x00FF;    //  topic Lengh LSB

	 int all_len = 4;
	 for(i=0;i<len;i++)
	 {
		 buffer[all_len] = topic[i];
	     all_len++;
	 }

	 len = lenpay;
	 for(i=0;i<len;i++)
	 {
		 buffer[all_len] = payload[i];
	     all_len++;
	 }

	 buffer[1]=all_len-2;
	//Serial.println();
	/*for(i=0;i<all_len;i++)
	{
		Serial.print(" ");
		Serial.print(buffer[i],16);
		//Ql_Debug_Trace("%02x",data[i]);
	}
	*/
	//Serial.println();
//	Serial.println(all_len);
	
	connected=write_tcp(buffer,all_len);
}
void GSMAxMQTT::publish(String topic , int data)
{
	publish(topic , String(data));
}
void GSMAxMQTT::publish(String topic , float data,unsigned char point)
{
	publish(topic , String(data,point));
}
void GSMAxMQTT::publish(String topic , String payload)
{
	
	char chartopic[topic.length()+2];
	char charpay[payload.length()+2];
	unsigned char i=0;
	for(i=0;i<topic.length();i++)
	{
		chartopic[i] = topic[i];
	}
	chartopic[i]=0;
	
	for(i=0;i<payload.length();i++)
	{
		charpay[i] = payload[i];
	}
	charpay[i]=0;
	
	publish(chartopic,topic.length(),charpay,payload.length());
}

void GSMAxMQTT:: clear_buffer()
{
	for(int i=0;i<MQTT_MAX_PACKET_SIZE;i++)
	{
		buffer[i]=0;
	}
	gsm.flush();
}

void GSMAxMQTT:: subscribe(char *topic,int topiclen)
{
	clear_buffer();   
    buffer[0]=0x82;
    buffer[1]=0x00; //Remaining Length
    buffer[2]=0x00;    // Packet Identifier MSB
    buffer[3]=0x02;    // Packet Identifier LSB

    int i=0;
    int len = topiclen;
    buffer[4]=(len>>8)&0x00FF;;    //  topic Lengh MSB
    buffer[5]=len&0x00FF;    //  topic Lengh LSB

    int all_len = 6;
     for(i=0;i<len;i++)
    {
        buffer[all_len] = topic[i];
         all_len++;
    }
    buffer[all_len] = 0;
    buffer[1] = all_len-1;
	connected=write_tcp(buffer,all_len+1);
}

void GSMAxMQTT:: subscribe(String topic)
{
	char chartopic[topic.length()+2];
	unsigned char i=0;
	for(i=0;i<topic.length();i++)
	{
		chartopic[i] = topic[i];
	}
	chartopic[i]=0;
	
	subscribe(chartopic,topic.length());
}

void GSMAxMQTT::Ping()
{
	buffer[0] = 0xC0;
	buffer[1] = 0x00;
	connected = write_tcp(buffer,2);
	//return(connected);
}
void GSMAxMQTT:: loop()
{
	int all_len =0 ;
	unsigned char buffer_sub[100];
	unsigned char sub_cnt=0;
	if(gsm.available())
	{
		String input = gsm.readStringUntil(',');
		//Serial.println(input);
		if(input.indexOf("+CIPRCV")!=-1)
		{
			Serial.println("data comming...");
			int index = input.indexOf(":");
			all_len = input.substring(index+1).toInt();
			//Serial.println("All length = "+String(all_len));
			sub_cnt=0;
			while(all_len)
			{
				if(gsm.available())
				{
					buffer_sub[sub_cnt] = gsm.read();
					//Serial.print("cnt"+String(sub_cnt)+"-> ");
					//Serial.println(buffer_sub[sub_cnt],HEX);
					sub_cnt++;
					all_len--;
					if(sub_cnt>=100)
						sub_cnt=0;
				}
			
			}
			if(buffer_sub[0]==0x30)
			{
				int sub_len = buffer_sub[2]<<4;
					sub_len |= buffer_sub[1];
				unsigned char topic_len = buffer_sub[3];
				String topic="";
				char payload[100];
				unsigned char pay_len=0;
				for(unsigned char i=0;i<sub_len-2;i++)
				{
					if(i< topic_len)
					{
						char c = buffer_sub[4+i];
						topic += c;
						//Serial.write(buffer_sub[4+i]);
					}
					else
					{
						payload[pay_len]= buffer_sub[4+i];
						pay_len++;
					}
				}
				//Serial.println(topic);
				(*callback)(topic,payload,pay_len);
			}
			if((buffer_sub[0]==0xD0))
			{
				//Serial.println("ping OK");
				connected = true;
			}
			
		}
		else
		{
			//Serial.println(input);
		}
	}
	
	unsigned long currentMillis = millis();
	if (currentMillis - pv_ping  >= interval_ping) 
	{
		//Serial.println("ping...");
		if(connected)
		Ping();
		pv_ping = currentMillis;
	}
}
bool GSMAxMQTT:: connectState()
{
	return(connected);
}
//unsigned long pv_ping = 0;  
//const long interval_ping = 10000; 