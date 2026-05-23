#include <Wire.h>
#define I2C_SlaveAdress1 0x18
#define I2C_SlaveAdress2 0x20
#define I2C_SDA 21
#define I2C_SCL 22
#define LED_PIN 15
uint8_t error = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(I2C_SDA,I2C_SCL);
  //delay(3000);
  i2cScanner();
  pinMode(LED_PIN, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  uint8_t bytesReceived = Wire.requestFrom(I2C_SlaveAdress1,4);
  if (bytesReceived > 0) {
    int8_t temp[10];
    for(int i = 0; i < bytesReceived; i++){
      //aca imprimo todos los bytes recibidos
      temp[i] = Wire.read();
      Serial.print("Byte: ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(temp[i]);
      Serial.println(" ");

    }
    if(temp[0] == 'o'){
      digitalWrite(LED_PIN, HIGH);   // enciende
    } else {
      digitalWrite(LED_PIN, LOW);    // apaga si no es 'o'
    }
  } else{
    //Serial.println("No data received");
  }
}
void i2cScanner(){
  // funcion para buscar los dispositivos que estan conectados
  byte error, address;
  int nDevices = 0;
  Serial.println("Scanning....");
  
  for (address = 1; address < 127; address++){
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0){
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    } else if(error == 4){
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }  
  if(nDevices == 0){
    Serial.println("No hay dispositivos");
  } else {
    Serial.println("done");
  }
}
