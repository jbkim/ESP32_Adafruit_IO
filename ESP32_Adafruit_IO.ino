
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "config.h"

#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include "images.h"

#include <DHT.h>                         //include Grove DHT library
#include <Adafruit_Sensor.h>
#include <DHT_U.h>


#define TouchPin    5
#define ledPin      2 // BuiltIn LED
#define BuzzerPin   4
#define DHT_TYPE    DHT11                 // DHT Sensor Type
#define DHTPin      18                   // DHT sensor pin
#define LightPin    35     //ADC7 inout at D35
  
// DHT dht(DHTPin,DHT_TYPE);
DHT_Unified dht(DHTPin, DHT_TYPE);
SSD1306Wire  display(0x3c, 22, 23);  // SDA, SCL

// float Temperature;
// float Humidity;

// button state
bool current = false;
bool last = false;

// light sensor state
int light_current = 0;
int light_last = -1;

// set up the 'digital' feed
AdafruitIO_Feed *digital = io.feed("digital");
// set up the 'digitalout' feed
AdafruitIO_Feed *digitalout = io.feed("digitalout");
// set up the 'analog' feed
AdafruitIO_Feed *analog = io.feed("analog");
// set up the 'temperature' and 'humidity' feeds
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");

void setup() {
  pinMode(TouchPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);
  pinMode(DHTPin,INPUT);             // sets the sensor pin as input

  dht.begin();
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  // display.drawString(0, 0, "Adafruit IO Test: " + String(millis()));
  display.drawString(0, 0, "SSID: " + String(WIFI_SSID));


  Serial.begin(115200);
  while(! Serial);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the 'digitalout' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  digitalout->onMessage(handleMessage);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  Serial.println(WiFi.localIP());
  
  digitalout->get();

}

void loop() {
  // clear the display
  display.clear();
  // display.drawString(0, 0, "Adafruit IO Test: " + String(millis()));
  // display.drawString(0, 10, "IP : " + WiFi.localIP());

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  dht.humidity().getEvent(&event);  

  float Temperature = event.temperature;
  float Humidity = event.relative_humidity;

// save temp & humidity to Adafruit IO
  temperature->save(Temperature);
  humidity->save(Humidity);

/*
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
*/
  if (isnan(Temperature) || isnan(Humidity))    // Check the return value.
  {
    Serial.println("Failed to read Temperature and Humidity from DHT Sensor");
  }
  else
  {
    Serial.print("Temperature: ");
    Serial.print(Temperature);
    Serial.print(" *C \t");
    Serial.print("Humidity: ");
    Serial.print(Humidity);
    Serial.println("%");
  }
  display.drawString(0, 20, "Temperature: "+ String(Temperature) + "*C");
  display.drawString(0, 30, "Humidity   : "+ String(Humidity) + "%");  
  
  aio_status_t aio_status = io.status();
  Serial.print("Status: "); Serial.println(aio_status);
  display.drawString(0, 40, "IO Status : ");
  switch (aio_status) {
     case AIO_IDLE:  display.drawString(40, 40, "IDLE"); break;
     case AIO_DISCONNECTED:
     case AIO_NET_DISCONNECTED:  display.drawString(40, 40, "DISCONNECT"); break;
     case AIO_NET_CONNECTED:
     case AIO_CONNECTED_INSECURE:
     case AIO_CONNECTED: display.drawString(40, 40, "CONNECTED"); break;
  }  
  // write the buffer to the display
  display.display();

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  // grab the current state of the loght sensor
  light_current = analogRead(LightPin);
  // if (light_current != light_last) {
  //   // save the current state to the analog feed
  //   Serial.print("sending -> ");
  //   Serial.println(light_current);
  //   analog->save(light_current);

  //   // store last loght sensor state
  //   light_last = light_current;
  // }

  // grab the current state of the button.
  // we have to flip the logic because we are
  // using a pullup resistor.
  if(digitalRead(TouchPin) == LOW) {
    current = true;  }
  else {
    // save the current state to the analog feed
    Serial.print("sending -> ");
    Serial.println(light_current);
    analog->save(light_current);

    current = false;
  }
    

  // return if the value hasn't changed
  if(current != last) {
    // save the current state to the 'digital' feed on adafruit io
    Serial.print("sending button -> ");
    Serial.println(current);
    digital->save(current);

    // store last button state
    last = current;
  }
    

  // =====================
  // int TouchInput = digitalRead(TouchPin);
  // if(TouchInput == 1) {
  //   digitalWrite(ledPin, HIGH);
  //   // digitalWrite(BuzzerPin, HIGH);
  // }
  // else {
  //   digitalWrite(ledPin, LOW);    
  //   // digitalWrite(BuzzerPin, LOW);      
  // }
  delay(5000);
}


// this function is called whenever an 'digital' feed message
// is received from Adafruit IO. it was attached to
// the 'digital' feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {

  Serial.print("received <- ");

  if(data->toPinLevel() == HIGH)
    Serial.println("HIGH");
  else
    Serial.println("LOW");


  digitalWrite(BuzzerPin, data->toPinLevel());
}


/*
void drawFontFaceDemo() {
    // Font Demo1
    // create more fonts at http://oleddisplay.squix.ch/
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Hello world");
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 10, "Hello world");
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 26, "Hello world");
}

void drawTextFlowDemo() {
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawStringMaxWidth(0, 0, 128,
      "Lorem ipsum\n dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore." );
}

void drawTextAlignmentDemo() {
    // Text alignment demo
  display.setFont(ArialMT_Plain_10);

  // The coordinates define the left starting point of the text
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 10, "Left aligned (0,10)");

  // The coordinates define the center of the text
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 22, "Center aligned (64,22)");

  // The coordinates define the right end of the text
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 33, "Right aligned (128,33)");
}

void drawRectDemo() {
      // Draw a pixel at given position
    for (int i = 0; i < 10; i++) {
      display.setPixel(i, i);
      display.setPixel(10 - i, i);
    }
    display.drawRect(12, 12, 20, 20);

    // Fill the rectangle
    display.fillRect(14, 14, 17, 17);

    // Draw a line horizontally
    display.drawHorizontalLine(0, 40, 20);

    // Draw a line horizontally
    display.drawVerticalLine(40, 0, 20);
}

void drawCircleDemo() {
  for (int i=1; i < 8; i++) {
    display.setColor(WHITE);
    display.drawCircle(32, 32, i*3);
    if (i % 2 == 0) {
      display.setColor(BLACK);
    }
    display.fillCircle(96, 32, 32 - i* 3);
  }
}

void drawProgressBarDemo() {
  int progress = (counter / 5) % 100;
  // draw the progress bar
  display.drawProgressBar(0, 32, 120, 10, progress);

  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, String(progress) + "%");
}

void drawImageDemo() {
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

Demo demos[] = {drawFontFaceDemo, drawTextFlowDemo, drawTextAlignmentDemo, drawRectDemo, drawCircleDemo, drawProgressBarDemo, drawImageDemo};
int demoLength = (sizeof(demos) / sizeof(Demo));
long timeSinceLastModeSwitch = 0;
*/


