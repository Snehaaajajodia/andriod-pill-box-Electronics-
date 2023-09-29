#include <TimeLib.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include <FirebaseESP8266.h>

#define FIREBASE_HOST "notifier-b8e6c-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "p7WWwy06nmu3XRfbWNccrh01cGmlNxzWAmWK3nHF"

#define REMINDER_PATH "/reminder"

LiquidCrystal_I2C lcd(0x3F, 16, 2);

int buzzer = 12;


long targetStamp;
String title;


FirebaseData firebaseData;

// Replace with your SSID and password
const char* ssid = "galaxy";
const char* password = "samsung4321";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

unsigned long epochTime; 

unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}


// Enter the desired target date and time


Ticker timer;

void setup() {

  Serial.begin(115200);

  pinMode(buzzer, OUTPUT);

  lcd.init();
  lcd.clear();
  lcd.backlight(); 
  
  lcd.setCursor(0, 0);
  lcd.print("    NOTIFIER    ");

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);


  }

  Serial.println();
  Serial.print("Connected to Wi-Fi. IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  // Set up the timer interrupt

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

 
}


void loop() {
  // Your other loop code (if any) goes here
  if (Firebase.getString(firebaseData, "/reminder/stamp")) {
    if (firebaseData.dataType() == "int") {
      targetStamp = firebaseData.intData();
      //Serial.print("Stamp Value: ");
      //Serial.println(targetStamp);
    }

  }

  if (Firebase.getString(firebaseData, "/title/name")){

    title = firebaseData.stringData();
    //Serial.println(title);
    

  }
  epochTime = getTime();

  Serial.print("Now: ");

  Serial.print(epochTime);

  Serial.print("  , ");

  Serial.print("Target: ");
  Serial.print(targetStamp);

  Serial.print("  ,");
  Serial.println(title);

  if (epochTime <= targetStamp){

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("    NOTIFIER    ");

    lcd.setCursor(0,1);
    lcd.print(targetStamp-epochTime);
  }


  if (epochTime >= targetStamp) {
    // Perform the action when the timer expires
    Serial.println("Timer expired!");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("    NOTIFIER    ");

    lcd.setCursor(0,1);
    lcd.print(title);

    tone(buzzer, 1000);
    delay(150);
    noTone(buzzer);
    delay(150);
    
    // Add your desired code here

    // To stop the timer after it triggers once
    timer.detach();
  }
}
