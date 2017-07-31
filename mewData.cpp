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

#include "mewData.h"


mew_Parameter::mew_Parameter(const char *id, float value, float (*refreshFunction)(), boolean active){
	_id = id;
    _value = value;
    _refreshFunction = refreshFunction;
	_active = active;
}

void mew_Parameter::setValue(float Value){
	_value = Value;
}

void mew_Parameter::setRefreshFunction (float (*refreshFunction)()){
	 _refreshFunction = refreshFunction;
}

void mew_Parameter::setId(const char* id){
	_id = id;
}

float mew_Parameter::getValue(){
	return _value;
}

const char* mew_Parameter::getId(){
	return _id;
}

void mew_Parameter::doRefresh(){
	_value = (*_refreshFunction)();
}

//-----------------------------------------------
//			MewData
//-----------------------------------------------


mewData::mewData(WiFiClient* mewClient, const char* Host, const char* Route, const char* Apikey){
	_isWiFi = true;
	_WifiClient = mewClient;
	_Apikey = Apikey;
	_Route = Route;
	_Host = Host;
	_UpdateTime = millis() + _UpdateRate;
    _paramsCount = 0;
	_debug = 1;
	DEBUG_MEW("Inicializado");
	if (WiFi.status() != WL_CONNECTED){
		DEBUG_MEW("no esta conectado!");
	}else{
		if (!_WifiClient->connect(_Host, _Port)) {
			DEBUG_MEW("Conexion fallida!");
		}else{
			DEBUG_MEW("Conectado!");
		}
	}
}

mewData::mewData(EthernetClient* mewClient, const char* Host, const char* Route, const char* Apikey, const byte* mac){

	_isWiFi = false;
	_EthClient = mewClient;
	_Apikey = Apikey;
	_Route = Route;
	_Host = Host;
	_UpdateTime = millis() + _UpdateRate;
    _paramsCount = 0;
	_debug = 1;
	DEBUG_MEW("Inicializando");
	if (Ethernet.begin(mac) == 0) {
    	DEBUG_MEW("Failed to configure Ethernet using DHCP");
  	}else{
		DEBUG_MEW("Conectando");
		if (!_EthClient->connect(_Host, _Port)) {
			DEBUG_MEW("Conexion fallida!");
		}else{
			DEBUG_MEW("Conectado!");
		}
	}
}




void mewData::set_Type(boolean isWiFi){
	_isWiFi = isWiFi;
}

void mewData::addParameter(mew_Parameter *p){
  _params[_paramsCount] = p;
  _paramsCount++;
  DEBUG_MEW("Parametro añadido");
  DEBUG_MEW(_paramsCount);
  DEBUG_MEW(p->getId());
}

void mewData::set_Apikey(const char *Apikey){
	_Apikey = Apikey;
}

void mewData::set_Route(const char *Route){
	_Route = Route;
}
void mewData::Run (boolean Run){
	_run = Run;
	_UpdateTime = millis() + _UpdateRate;
}
void mewData::set_Host(const char *Host){
	_Host = Host;
}

void mewData::setUpdateRate(unsigned int time){
	_UpdateRate = time*1000; //Segundos
}

void mewData::setDebug(boolean Debug){
	_debug = Debug;
}
void mewData::sendReset(boolean sendReset){
	_sendReset = sendReset;
}
void mewData::sendUptime(boolean Uptime){
	_sendUptime = Uptime;
}
void mewData::sendRam(boolean sendRam){
	_sendRam = sendRam;
}
void mewData::refreshParams(){
	for (int i = 0; i < _paramsCount; i++) {
		if (_params[i] == NULL) {
			break;
		}
	if (_params[i]->_active){
	_params[i]->doRefresh();
	DEBUG_MEW("refrescando parametro");
	DEBUG_MEW(_params[i]->getId());
	DEBUG_MEW(_params[i]->getValue());
	}
}
}
void mewData::Work(){
	if (_run){
    
		if (millis() > _UpdateTime ){
			this->refreshParams();
			this->Update();
			_UpdateTime = millis() + _UpdateRate;
		}
		if (_isWiFi){
			while(_WifiClient->available()){
    			yield();
    			DEBUG_MEW(_WifiClient->readStringUntil('k'));
    			delay(0);
			}
		}else{
			while(_EthClient->available()){
    			yield();
    			DEBUG_MEW(_EthClient->readStringUntil('k'));
    			delay(0);
			}
		}
	}
}

void mewData::Update(){
	DEBUG_MEW("Updating");
	_Url = "";
	_Url += _Route;
	_Url += _Apikey;
	_Url += "&json={";
	if(_sendReset){
	if(_Reset){
		_Reset--;
		_Url += "R:";
		_Url += _Reset;
		_Url += ',';
	}}
	if(_sendUptime){
		_Url += "UT:";
		_Url += float(millis())/(3600000);
		_Url += ',';
	}
	if(_sendRam){
		_Url += "RM:";
		_Url += float(ESP.getFreeHeap())/1024;
		_Url += ',';
	}

	for (int i = 0; i < _paramsCount; i++) {
		if (_params[i] == NULL) {
			break;
		}
	_Url += _params[i]->getId();
	_Url += ':';
	_Url += _params[i]->getValue();
	if (i < _paramsCount-1){
		_Url +=',';
	}
}
	if (_Url.endsWith(",")){
		_Url.remove(_Url.length()-1);
	}
	_Url +='}';
	DEBUG_MEW(_Url);
	if (_isWiFi){
		if(_WifiClient->connected()){
			_WifiClient->print(String("GET ") + _Url + " HTTP/1.1\r\n" +
				"Host: " + _Host + "\r\n" +
				"Connection: close\r\n\r\n");
		}else{
			if (!_WifiClient->connect(_Host, _Port)) {
				DEBUG_MEW("Conexion fallida!");
			}else{
				DEBUG_MEW("Conectado!");
				_WifiClient->print(String("GET ") + _Url + " HTTP/1.1\r\n" +
				"Host: " + _Host + "\r\n" +
				"Connection: close\r\n\r\n");
			}
		}
	}else{
		if(_EthClient->connected()){
			_EthClient->print(String("GET ") + _Url + " HTTP/1.1\r\n" +
			"Host: " + _Host + "\r\n" +
			"Connection: close\r\n\r\n");
		}else{
			if (!_EthClient->connect(_Host, _Port)) {
				DEBUG_MEW("Conexion fallida!");
			}else{
				DEBUG_MEW("Conectado!");
				_EthClient->print(String("GET ") + _Url + " HTTP/1.1\r\n" +
				"Host: " + _Host + "\r\n" +
				"Connection: close\r\n\r\n");
			}
		}
	}
}


template <typename mGeneric>
void mewData::DEBUG_MEW(mGeneric text) {
  if (_debug) {
    Serial.print("*mew: ");
    Serial.println(text);
  }
}
