/**************************************************************
   mewData Library
   fng Design 2016
   v 0.1
   Library encargada de gestionar la conexion y el envio de 
   datos al server. Funciona con nodos Esp8266.
   Requiere conexion previa a la red WiFi, generalmente lograda
   con fngManager. Probablemente se unan ambas libs a futuro para
   que gestione tambien la conexion Wifi y los parametros a enviar.
   
 **************************************************************/

#ifndef mewData_h
#define mewData_h

#include <ESP8266WiFi.h>
#include <memory>

#define MEWDATA_MAX_PARAMS 5

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
	private:
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
		WiFiClient* _WiFiClient;
		boolean _debug=false;
		boolean _run = false;
		boolean _sendReset = true;
		boolean _sendUptime = true;
		boolean _sendRam = false;
		template <typename mGeneric>
		void DEBUG_MEW(mGeneric text);	
};



#endif