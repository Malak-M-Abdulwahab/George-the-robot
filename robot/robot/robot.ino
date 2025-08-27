#define REMOTEXY_MODE__ESP32CORE_BLE
#define REMOTEXY_BLUETOOTH_NAME "RemoteXY" // RemoteXY connection settings 
#define MIC_PIN 4 // Microphone pin
#define WIFI_SSID " "
#define WIFI_PASSWORD " "
#define TTS_MODEL 0
#define AUDIO_FILE "/Audio.wav" // File of audio saved
#define TTS_GOOGLE_LANGUAGE "en-IN"
#define pin_I2S_DOUT 25  // 3 pins for I2S Audio Output (Schreibfaul1 audio.h Library)
#define pin_I2S_LRC 26
#define pin_I2S_BCLK 27

#include <BLEDevice.h>
#include <ESP32Servo.h>
#include <RemoteXY.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SD.h>
//#include <Audio.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <SimpleTimer.h>

#pragma pack(push, 1)  // RemoteXY GUI configuration  
uint8_t RemoteXY_CONF[] =
  { 255,19,0,6,0,197,1,19,0,0,0,0,16,1,106,200,6,1,0,0,
  0,0,0,10,0,1,9,15,88,23,5,244,31,0,1,9,50,88,23,5,
  244,31,0,1,9,85,88,23,5,244,31,0,1,9,120,88,23,5,244,31,
  0,1,9,155,88,23,5,244,31,0,131,9,15,88,23,5,244,245,16,70,
  114,101,101,32,87,97,108,107,0,166,10,131,9,50,88,23,5,244,245,16,
  70,111,108,108,111,119,32,76,105,110,101,0,154,10,131,9,85,88,23,5,
  244,245,16,67,97,114,114,121,32,79,98,106,101,99,116,0,106,10,131,9,
  120,88,23,5,244,245,16,65,115,107,32,71,101,111,114,103,101,0,170,6,
  131,9,155,88,23,5,244,245,16,68,97,110,99,101,32,33,33,33,33,0,
  170,9,3,0,1,10,183,84,15,5,244,31,0,131,10,183,84,15,5,244,
  245,16,66,97,99,107,0,169,10,5,13,27,80,80,32,243,244,16,3,0,
  1,10,183,84,15,5,244,31,0,131,10,183,84,15,5,244,245,16,66,97,
  99,107,0,169,10,2,12,27,82,39,0,244,243,16,16,79,78,0,79,70,
  70,0,4,0,1,11,183,84,15,5,244,31,0,131,11,183,84,15,5,244,
  245,16,66,97,99,107,0,169,10,5,13,27,80,80,32,243,244,16,1,9,
  113,25,25,0,244,16,67,97,114,114,121,0,6,0,1,11,182,84,15,5,
  244,31,0,131,11,182,84,15,5,244,245,16,66,97,99,107,0,169,10,69,
  12,22,21,21,0,243,10,39,23,53,21,53,244,245,16,83,112,101,97,107,
  0,16,83,116,111,112,0,74,8,50,89,30,12,244,244,16,64,0,1,16,
  93,72,72,0,244,16,68,97,110,99,101,33,33,0,5,0,1,11,183,84,
  15,5,244,31,0,131,11,183,84,15,5,244,245,16,66,97,99,107,0,169,
  10,69,10,22,21,21,0,243,10,37,23,53,21,53,244,245,16,83,112,101,
  97,107,0,16,83,116,111,112,0,74,5,53,96,118,12,244,244,16,64,0 };
  
struct 
{
    // input variables
  uint8_t button1; // =1 if button pressed, else =0
  uint8_t button2; // =1 if button pressed, else =0
  uint8_t button3; // =1 if button pressed, else =0
  uint8_t button4; // =1 if button pressed, else =0
  uint8_t button5; // =1 if button pressed, else =0
  uint8_t back1; // =1 if button pressed, else =0
  int8_t move1; // from -100 to 100
  int8_t move2; // from -100 to 100
  uint8_t back2; // =1 if button pressed, else =0
  uint8_t start; // =1 if switch ON and =0 if OFF
  uint8_t back3; // =1 if button pressed, else =0
  int8_t move3; // from -100 to 100
  int8_t move4; // from -100 to 100
  uint8_t carry; // =1 if button pressed, else =0
  uint8_t back5; // =1 if button pressed, else =0
  uint8_t speakSwitch2; // =1 if state is ON, else =0
  uint8_t danceButton; // =1 if button pressed, else =0
  uint8_t back4; // =1 if button pressed, else =0
  uint8_t speakSwitch1; // =1 if state is ON, else =0
    // output variables
  int16_t sound2; // =0 no sound, else ID of sound, =1001 for example, look sound list in app
  uint8_t string2; // from 0 to 1
  int16_t sound1; // =0 no sound, else ID of sound, =1001 for example, look sound list in app
  uint8_t string1; // from 0 to 1
    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)

Servo myServo;  // Create a Servo object
const char* deepGramApiKey = " ";
const char* geminiApiKey = " ";
String text;
String filteredAnswer = " ";
String repeat;
SimpleTimer Timer;
float batteryVoltage;

void setup() 
{
  Serial.begin(9600);
  RemoteXY_Init (); 
  myServo.attach(5);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
  }

  SD.begin();
  I2S_Record_Init();
  audio_play.setVolume(21);
}

int getPageNumber() // Accessing each page separately
{
  return RemoteXY.button1*1+RemoteXY.button2*2+RemoteXY.button3*3+RemoteXY.button4*4+RemoteXY.button5*5;
}

void walk() // Walk function in the Free Walk page
{
  while(!RemoteXY.back1)
  {
    uint8_t servoAngle = map(RemoteXY.move1, -100, 100, 0, 180);
    servoAngle = constrain(servoAngle, 0, 180); // Constrain the angle to ensure it stays within 0-180 degrees
    myServo.write(servoAngle);
    RemoteXY_delay(200 - abs(servoAngle));
  }
}

void lineFollow()
{
  while(!RemoteXY.back2)
  {

  }
}

void carryAndMove()
{
  while(!RemoteXY.back3)
  {
    uint8_t servoAngle = map(RemoteXY.move3, -100, 100, 0, 180);
    servoAngle = constrain(servoAngle, 0, 180); // Constrain the angle to ensure it stays within 0-180 degrees
    myServo.write(servoAngle);
    RemoteXY_delay(200 - abs(servoAngle));

    if(RemoteXY.carry)
    {

    }
  }
}

void speakTextInChunks(String text, int maxLength) {
  int start = 0;
  while (start < text.length()) {
    int end = start + maxLength;

    // Ensure we don't split in the middle of a word
    if (end < text.length()) {
      while (end > start && text[end] != ' ' && text[end] != '.' && text[end] != ',') {
        end--;
      }
    }

    // If no space or punctuation is found, just split at maxLength
    if (end == start) {
      end = start + maxLength;
    }

    String chunk = text.substring(start, end);
    audio_play.connecttospeech(chunk.c_str(), TTS_GOOGLE_LANGUAGE);

    while (audio_play.isRunning()) {
      audio_play.loop();
      if (digitalRead(pin_RECORD_BTN) == LOW) {
        break;
      }
    }

    start = end + 1;  // Move to the next part, skipping the space
                      // delay(200);       // Small delay between chunks
  }
}

void askGeorge() // Talk to our robot, George!
{
  while(!RemoteXY.back4)
  {
    while(RemoteXY.speakSwitch1)
    {
     if (audio_play.isRunning()) 
     {
      audio_play.connecttohost("");
     }
     Record_Start(AUDIO_FILE);//Start Recording
    }
    while(!RemoteXY.speakSwitch1)
    {
      float recorded_seconds;
      if (Record_Available(AUDIO_FILE, &recorded_seconds))  //  true once when recording finalized (.wav file available)
      {
        if (recorded_seconds > 0.4)  // ignore short btn TOUCH
        {
          String transcription = SpeechToText_Deepgram(AUDIO_FILE);
  
          Serial.println(transcription);
          if (transcription == "") {
            led_RGB(0, 0, 255);
            if (TTS_MODEL == 1)
              // audio_play.openai_speech(OPENAI_KEY, "tts-1", again, "shimmer", "mp3", "1");  //ONYX,shimmer,alloy (Uncomment this to use OpenAI TTS)
            else
              speakTextInChunks(again, 93);  // ( Uncomment this to use Google TTS )
            while (audio_play.isRunning())  // wait here until finished (just for Demo purposes, before we play Demo 4)
            {
              audio_play.loop();
            }
          }
          
          WiFiClientSecure client;
          client.setInsecure();  // Disable SSL verification for simplicity (not recommended for production)
          String Answer = "";    // Declare Answer variable here
  
          text = "";
  
          if (client.connect("generativelanguage.googleapis.com", 443)) {
            String url = "/v1beta/models/gemini-1.5-flash:generateContent?key=" + String(geminiApiKey);
  
            String payload = String("{\"contents\": [{\"parts\":[{\"text\":\"" + transcription + "\"}]}],\"generationConfig\": {\"maxOutputTokens\": " + OpenAI_Max_Tokens + "}}");
  
  
            // Send the HTTP POST request
            client.println("POST " + url + " HTTP/1.1");
            client.println("Host: generativelanguage.googleapis.com");
            client.println("Content-Type: application/json");
            client.print("Content-Length: ");
            client.println(payload.length());
            client.println();
            client.println(payload);
  
            // Read the response
            String response;
            while (client.connected()) {
              String line = client.readStringUntil('\n');
              if (line == "\r") {
                break;
              }
            }
  
            // Read the actual response
            response = client.readString();
            parseResponse(response);
          };
          else 
          {
            Serial.println("Connection failed!");
          }
  
          client.stop();  // End the connection
  
          if (filteredAnswer != "")  // we found spoken text .. now starting Demo examples:
          {
            if (TTS_MODEL == 1)
              // audio_play.openai_speech(OPENAI_KEY, "tts-1", filteredAnswer.c_str(), "shimmer", "mp3", "1");  //ONYX,shimmer,alloy (Uncomment this to use OpenAI TTS)
            else
              speakTextInChunks(filteredAnswer, 93);  // ( Uncomment this to use Google TTS )
          }
        }
      }
    }
  }
}

void parseResponse(String response) {
  repeat = "";
  // Extract JSON part from the response
  int jsonStartIndex = response.indexOf("{");
  int jsonEndIndex = response.lastIndexOf("}");

  if (jsonStartIndex != -1 && jsonEndIndex != -1) {
    String jsonPart = response.substring(jsonStartIndex, jsonEndIndex + 1);
    // Serial.println("Clean JSON:");
    // Serial.println(jsonPart);

    DynamicJsonDocument doc(1024);  // Increase size if needed
    DeserializationError error = deserializeJson(doc, jsonPart);

    if (error) {
      Serial.print("DeserializeJson failed: ");
      Serial.println(error.c_str());
      return;
    }

    if (doc.containsKey("candidates")) {
      for (const auto& candidate : doc["candidates"].as<JsonArray>()) {
        if (candidate.containsKey("content") && candidate["content"].containsKey("parts")) {

          for (const auto& part : candidate["content"]["parts"].as<JsonArray>()) {
            if (part.containsKey("text")) {
              text += part["text"].as<String>();
            }
          }
          text.trim();
          // Serial.print("Extracted Text: ");
          // Serial.println(text);
          filteredAnswer = "";
          for (size_t i = 0; i < text.length(); i++) {
            char c = text[i];
            if (isalnum(c) || isspace(c) || c == ',' || c == '.' || c == '\'') {
              filteredAnswer += c;
            } else {
              filteredAnswer += ' ';
            }
          }
          // filteredAnswer = text;
          // Serial.print("FILTERED - ");
          //Serial.println(filteredAnswer);

          repeat = filteredAnswer;
        }
      }
    } else {
      Serial.println("No 'candidates' field found in JSON response.");
    }
  } else {
    Serial.println("No valid JSON found in the response.");
  }
}

void dance()
{
  while(!RemoteXY.back5)
  {
    while(RemoteXY.speakSwitch2)
    {
      askGeorge();

      if (responseText.length() > 0) 
      {
        playTextAsAudio(responseText);
      } 
      else 
      {

      }
      if(RemoteXY.danceButton)
      {

      }
    }
  }
}

void loop() 
{  
  RemoteXY_Handler ();
  switch(getPageNumber())
  {
    case 1:
      walk();
      break;

    case 2:
      lineFollow();
      break;

    case 3:
      carryAndMove();
      break;

    case 4:
      askGeorge();
      break;

    case 5:
      dance();
      break;
  }
}
