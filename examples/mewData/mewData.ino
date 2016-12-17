#include <ESP8266WiFi.h>
#include <mewData.h>


const char* ssid     = "fngCenter";
const char* password = "fermatfueungranmatematico";
mew_Parameter Puerta("P01",0,getMillValue,true);
mew_Parameter Puerta2("P02",0,getMillValue2,true);
WiFiClient WiFiclient;


void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
Serial.println("\n reset");
WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  mewData mewMaster(&WiFiclient,"mew.ezver.com.ar","/api/post.json?apikey=","6662ead9bc638720bdb8310749c08d54");
  mewMaster.addParameter(&Puerta);
  mewMaster.addParameter(&Puerta2);
//  mewMaster.setUpdateRate(5);
//  mewMaster.setDebug(false);
  mewMaster.Run(true);
  while(1){
    mewMaster.Work();
    yield();

}
}
float getMillValue(){
  return millis()/1000;
}
float getMillValue2(){
  return float(millis())/2000;
}
