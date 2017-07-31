/**************************************************************
   mewData Library
   fng Design 2017

 v 0.3   07/2017
   Sobrecarga de la inicializacion para inicializar con UIPEthernet
   Agregado de funciones de Serial para levantar datos de fngBus
   Funciona con master en esp8266 o AVR.
   

   v 0.2
   Library encargada de gestionar la conexion y el envio de
   datos al server. Funciona con nodos Esp8266.
   Requiere conexion previa a la red, pudiendo ser
	 Wifi o ethernet (enc28j60).
 **************************************************************/


#ifndef mewData_h
#define mewData_h

#include <ESP8266WiFi.h>
#include <memory>
#include <UIPEthernet.h>


#define MEWDATA_MAX_PARAMS 25

class mew_Parameter {
  public:
    mew_Parameter(const char *id, float value, float (*refreshFunction)(), boolean active);

	void setValue(float Value);
	void setRefreshFunction (float (*refreshFunction)());
	void setId(const char* id);
	void doRefresh();
	float getValue();
	const char* getId();
	void Active(boolean Active);

  private:
    const char *_id;
    float _value;
	  float (*_refreshFunction)();
	  boolean _active = false;
  friend class mewData;
};


class mewData{
	public:


		mewData(WiFiClient* mewClient,const char* Host, const char* Route, const char* Apikey);
    	mewData(EthernetClient* mewClient,const char* Host, const char* Route, const char* Apikey,const byte* mac);  
		void addParameter(mew_Parameter *p);
		void set_Apikey(const char *Apikey);
		void set_Route(const char *Route);
		void set_Host(const char *Host);
		void Update();
		void refreshParams();
		void setUpdateRate(unsigned int time);
		void Work();
		void setDebug(boolean Debug);
		void Run (boolean Run);
		void sendReset(boolean sendReset);
		void sendUptime(boolean Uptime);
		void sendRam(boolean sendRam);
		void set_Type(boolean isWiFi);
	private:
		boolean _isWiFi = false;
		int _Port = 80;
		const char* _Apikey;
		String _Url;
		const char* _Route;
		const char* _Host;
		unsigned long _UpdateTime;
		unsigned int _UpdateRate = 10000;
        int           _paramsCount = 0;
		unsigned int	_Reset = 5;
        mew_Parameter* _params[MEWDATA_MAX_PARAMS];
		WiFiClient* _WifiClient;
		EthernetClient* _EthClient;
		boolean _debug=true;
		boolean _run = false;
		boolean _sendReset = true;
		boolean _sendUptime = true;
		boolean _sendRam = false;
		template <typename mGeneric>
		void DEBUG_MEW(mGeneric text);
};



#endif
