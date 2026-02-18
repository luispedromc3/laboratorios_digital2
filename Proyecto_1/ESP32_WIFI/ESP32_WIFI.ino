

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"
//---------------------------UART---------------------
#include <HardwareSerial.h>
#define RXD2 16
#define TXD2 17
// this int will hold the current count for our sketch
int count = 0;
int valor_T = 0;
int valor_H = 0;
int valor_P = 0;
char buffer[20];
int bufferIndex = 0;
int contador_publicar = 0;

#define IO_LOOP_DELAY 5000
unsigned long lastUpdate = 0;
int valor_led;
int bandera_modo = 0;
// set up the 'counter' feed
AdafruitIO_Feed *LED = io.feed("led");
AdafruitIO_Feed *temperatura = io.feed("temperatura");
AdafruitIO_Feed *peso = io.feed("peso");
AdafruitIO_Feed *humedad = io.feed("humedad");
AdafruitIO_Feed *servo = io.feed("servo");
AdafruitIO_Feed *stepper = io.feed("stepper");
AdafruitIO_Feed *dc = io.feed("dc");

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("SETUP OK");
  delay(500);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("UART2 OK");

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();
  Serial.println();
  Serial.println(io.statusText());
  //counter->onMessage(handleMessage);
  servo->onMessage(handleMessageServo);
  LED->onMessage(handleMessageLED);
  stepper->onMessage(handleMessageStepper);
  dc->onMessage(handleMessageDC);


  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    Serial.println();
    Serial.println(io.statusText());
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  //Suscribirme a los canales que quiero recibir de adafruit
  servo->get();
  LED->get();

}
 
void loop() {
  io.run();
  
 while (Serial2.available()) {
    char c = Serial2.read();

    if (c == '\n') {
      buffer[bufferIndex] = '\0';

      if (bufferIndex > 1) {
        char tipo = buffer[0];
        int valor = atoi(&buffer[1]);

        switch (tipo) {
          case 'T': valor_T = valor; break;
          case 'P': valor_P = valor; break;
          case 'H': valor_H = valor; break;
        }
      }

      bufferIndex = 0;  // reiniciar buffer
    }
    else {
      if (bufferIndex < sizeof(buffer) - 1) {
        buffer[bufferIndex++] = c;
      }
    }
  }

  if (millis() > (lastUpdate + IO_LOOP_DELAY)) {
    //valor_led = 0;
    //LED->save(valor_led);
    //enviamos valores de sensores
    //Serial.print("T:");
    //temperatura->save(valor_T);
    //Serial.print("P:");
    //peso->save(valor_P);
    //Serial.print("H:");
    //humedad->save(valor_H);
    switch (contador_publicar) {

      case 0:
        Serial.print("T:");
        temperatura->save(valor_T);
        contador_publicar++;
        break;

      case 1:
        Serial.print("P:");
        peso->save(valor_P);
        contador_publicar++;
        break;

      case 2:
        Serial.print("H:");
        humedad->save(valor_H);
        contador_publicar = 0;
        break;
      default:
        Serial.println("Valor fuera de rango");
        break;
    }
    count++;

    // after publishing, store the current time
    lastUpdate = millis();
    
  }

}

// this function is called whenever a 'counter' message
// is received from Adafruit IO. it was attached to
// the counter feed in the setup() function above.
void handleMessageServo(AdafruitIO_Data *data) {
  int servo = atoi(data->value());
  if (bandera_modo){
    if(servo){
      //si estamos en modo manual y queremos cerrar el servo, le mando 1
      Serial2.print("S1.");
    }else{
      //si estamos en modo manual y queremos abrir el servo, le mando 0
      Serial2.print("S0.");
    }
    
  }
  Serial.print("received SERVO <- ");
  Serial.println(data->value());

}
void handleMessageLED(AdafruitIO_Data *data) {
  int dato = atoi(data->value());
  if (dato == 1) {
    bandera_modo = 1;
    Serial2.write("M1.");
  } else{
    bandera_modo = 0;
    Serial2.write("M0.");
  }
  Serial.print("received MODO<- ");
  Serial.println(data->value());

}
void handleMessageStepper(AdafruitIO_Data *data){
  int dato = atoi(data->value());
  if(bandera_modo){
    if(dato == 1){
      Serial2.write("s1.");
    } else if (dato == 2){
      Serial2.write("s2.");
    } else if (dato == 3){
      Serial2.write("s3.");
    } else if (dato == 4){
      Serial2.write("s4.");
    }
  }
}
void handleMessageDC(AdafruitIO_Data *data){
  int dato = atoi(data->value());
  if(bandera_modo){
    if(dato == 1){
      Serial2.write("d1.");
    } else if (dato == 2){
      Serial2.write("d2.");
    } else if (dato == 3){
      Serial2.write("d3.");
    } else if (dato == 4){
      Serial2.write("d4.");
    }
  }
}
