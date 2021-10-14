#include <Adafruit_NeoPixel.h>

#define LED_PIN 6
#define LED_COUNT 30

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

// Declare colors
uint32_t turnOff = strip.Color(0, 0, 0, 0);
uint32_t red = strip.Color(64, 0, 0, 0);
uint32_t green = strip.Color(0, 64, 0, 0);
uint32_t blue = strip.Color(0, 0, 64, 0);
uint32_t white = strip.Color(0, 0, 0, 64);
uint32_t pink = strip.Color(199,21,133, 0);
uint32_t greenishwhite = strip.Color(0, 64, 0, 64);

// Declare array for led sections (First LED, number of LEDs)
int ledsOven[2] = {0, 8};
int ledsSink[2] = {8, 14};
int ledsMicro[2] = {22, 8};

int currentMode = -1;
int oldMode = 0;
int modeOven = 0;
int modeSink = 0;
int modeMicro = 0;

void setup() {
  Serial.begin(9600);
  strip.begin();
}

void loop() {
  serialData();
  
  if (currentMode != oldMode) {
    onNewMode();
    staticModes();
  }

  dynamicModes();
}

void serialData() {
  if (Serial.available() > 0) {
    int input = Serial.parseInt();
  
    if (input != 0) {
      currentMode = input;
      modeOven = (input / 100) % 10;
      modeSink = (input / 10) % 10;
      modeMicro = input % 10;
    
      Serial.print("New mode");
      Serial.print("; Oven = ");
      Serial.print(modeOven, DEC);
      Serial.print("; Sink = ");
      Serial.print(modeSink, DEC);
      Serial.print("; Micro = ");
      Serial.println(modeMicro, DEC);
    }
  }
}

void onNewMode() {
  oldMode = currentMode;
  strip.clear();
}

void staticModes() {
  if (currentMode == 1) {
    setOvenColor(pink);
    setSinkColor(green);
    setMicroColor(blue);
  } else if (currentMode == 9) {
    setOvenColor(turnOff);
    setSinkColor(turnOff);
    setMicroColor(turnOff);  
  }
}

void dynamicModes() {
  if (modeMicro == 2) {
    Strobe(0xff, 0xff, 0xff, 10, 50, 1000);
  } else if (modeMicro == 3) {
    CylonBounce(0xff, 0, 0, 4, 20, 50);
  } else if (modeMicro == 4) {
    Sparkle(0xff, 0xff, 0xff, 70);
  } else if (modeMicro == 5) {
    meteorRain(0xff,0xff,0xff,20, 64, true, 30);
  }
}

void setOvenColor(uint32_t color) {
  strip.fill(color, ledsOven[0], ledsOven[1]);
  strip.show();
}

void setSinkColor(uint32_t color) {
  strip.fill(color, ledsSink[0], ledsSink[1]);
  strip.show();
}

void setMicroColor(uint32_t color) {
  strip.fill(color, ledsMicro[0], ledsMicro[1]);
  strip.show();
}

void turnAllOff() {
  strip.clear();
}

// https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/

void showStrip() {
   strip.show();
}

void setAll(byte red, byte green, byte blue) {
  uint32_t color = strip.Color(red, green, blue, 0);
  strip.fill(color, 0, LED_COUNT);
}

void setPixel(int pixel, byte red, byte green, byte blue) {
  uint32_t color = strip.Color(red, green, blue);
  strip.setPixelColor(pixel, color);
}

void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause){
  for(int j = 0; j < StrobeCount; j++) {
    setAll(red,green,blue);
    showStrip();
    delay(FlashDelay);
    setAll(0,0,0);
    showStrip();
    delay(FlashDelay);
  }
 
 delay(EndPause);
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){

  for(int i = 0; i < LED_COUNT-EyeSize-2; i++) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for(int i = LED_COUNT-EyeSize-2; i > 0; i--) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  
  delay(ReturnDelay);
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(LED_COUNT);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel,0,0,0);
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  setAll(0,0,0);
  
  for(int i = 0; i < LED_COUNT+LED_COUNT; i++) {
    
    
    // fade brightness all LEDs one step
    for(int j=0; j<LED_COUNT; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
    
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <LED_COUNT) && (i-j>=0) ) {
        setPixel(i-j, red, green, blue);
      } 
    }
   
    showStrip();
    delay(SpeedDelay);
  }
}

void fadeToBlack(int ledNo, byte fadeValue) {
  // NeoPixel
  uint32_t oldColor;
  uint8_t r, g, b;
  int value;
  
  oldColor = strip.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
  g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
  b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
  
  strip.setPixelColor(ledNo, r,g,b);
}
