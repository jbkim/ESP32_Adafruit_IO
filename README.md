# ESP32_Adafruit_IO
ESP32 IoT service test - Adafruit IO

## Hardware 연결 정보
- Connected Sensors & Input : 대쉬보드에 주기적으로 정보를 디스플레이
    - temp, humidity 센서
    - Touch 입력
- Connected actuators
    - Buzzzer : 대쉬보드에서 컨트롤 가능
- OLED Disply

## Firmware 
### 사용자 정보
config.h 파일에서 IO의 사용자 이름과 키값, 연결할 WiFi정보의 값을 적는다.
~~~cpp
    #define IO_USERNAME    "xxxxx"
    #define IO_KEY         "xxxxxxxxxxxxxxxxxxxxxxxxx"
    #define WIFI_SSID       "xxxxx"
    #define WIFI_PASS       "xxxxx"
~~~
### 피드 설정
~~~cpp
    // set up the 'digital' feed
    AdafruitIO_Feed *digital = io.feed("digital");

    // set up the 'digitalout' feed
    AdafruitIO_Feed *digitalout = io.feed("digitalout");

    // set up the 'analog' feed
    AdafruitIO_Feed *analog = io.feed("analog");

    // set up the 'temperature' and 'humidity' feeds
    AdafruitIO_Feed *temperature = io.feed("temperature");
    AdafruitIO_Feed *humidity = io.feed("humidity");
~~~
### 상태정보 읽기
~~~cpp
  aio_status_t aio_status = io.status();
~~~
### 메시지 처리
~~~cpp
void handleMessage(AdafruitIO_Data *data) {
  Serial.print("received <- ");
  if(data->toPinLevel() == HIGH)
    Serial.println("HIGH");
  else
    Serial.println("LOW");
  digitalWrite(BuzzerPin, data->toPinLevel());
}
~~~
### 아날로그, 디지털 피드 업데이트
~~~cpp
  // grab the current state of the button.
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
~~~