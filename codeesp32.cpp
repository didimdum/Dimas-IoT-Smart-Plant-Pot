#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define PIN_LED     23
#define NUM_LEDS    8
#define SOIL_PIN    34
#define LDR_PIN     22

const char* ssid = "mieayam";
const char* password = "nasigoreng";
const char* serverURL = "http://10.223.136.140:5000/update";

Adafruit_NeoPixel strip(NUM_LEDS, PIN_LED, NEO_GRB + NEO_KHZ800);

void pulsingRed();
void smoothGradient(int soil);
void softRainbow();
void breathingPurpleBlue();
void setColor(int r, int g, int b);
void sendToServer(int soil, int light);

void setup() {
  Serial.begin(115200);

  strip.begin();
  strip.show();
  strip.setBrightness(255);

  pinMode(LDR_PIN, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.println(WiFi.localIP());
}

void loop() {
  int soilValue = analogRead(SOIL_PIN);
  int soilPercent = map(soilValue, 4095, 1200, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  int lightState = digitalRead(LDR_PIN);

  Serial.print("Soil: ");
  Serial.print(soilPercent);
  Serial.print("% | Light: ");
  Serial.println(lightState == 1 ? "Gelap" : "Terang");

  sendToServer(soilPercent, lightState);

  //lampu ungu pas malem
  if (lightState == 1) {
    breathingPurpleBlue();
  }
  //kalo siang
  else {
    if (soilPercent < 20) {
      pulsingRed();
    }
    else if (soilPercent < 60) {
      smoothGradient(soilPercent);
    }
    else {
      softRainbow();
    }
  }

  delay(100);
}

// warna
void setColor(int r, int g, int b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

// red pulsing

void pulsingRed() {
  for (int b = 0; b <= 150; b += 5) {
    setColor(b, 0, 0);
    delay(8);
  }
  for (int b = 150; b >= 0; b -= 5) {
    setColor(b, 0, 0);
    delay(8);
  }
}

// gradient
void smoothGradient(int soil) {
  float t = (soil - 20) / 40.0;
  int r, g;

  if (t < 0.5) {
    float k = t / 0.5;
    r = 255;
    g = 255 * k;
  } else {
    float k = (t - 0.5) / 0.5;
    r = 255 * (1 - k);
    g = 255;
  }

  setColor(r, g, 0);
}

// rainbow

void softRainbow() {
  static uint16_t j = 0;

  for (int i = 0; i < NUM_LEDS; i++) {
    int pixelIndex = (i * 256 / NUM_LEDS) + j;
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelIndex)));
  }
  strip.show();

  j += 2;
  delay(20);
}

// malem

void breathingPurpleBlue() {
  const int maxBrightness = 40;

  for (int b = 0; b <= maxBrightness; b++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color(b * 2, 0, b * 4));
    }
    strip.show();
    delay(25);
  }

  for (int b = maxBrightness; b >= 0; b--) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, strip.Color(b * 2, 0, b * 4));
    }
    strip.show();
    delay(25);
  }
}

// code kirim ke flask

void sendToServer(int soil, int light) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{";
    jsonData += "\"soil\":" + String(soil) + ",";
    jsonData += "\"light\":" + String(light);
    jsonData += "}";

    int httpResponseCode = http.POST(jsonData);
    Serial.print("HTTP Response: ");
    Serial.println(httpResponseCode);

    http.end();
  }
}
