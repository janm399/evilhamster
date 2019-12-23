#include <Arduino.h>

// Rainbow color changing RGB leds example
// I am using common cathode RGB leds
#define PIN_RED1 3
#define PIN_GREEN1 5
#define PIN_BLUE1 6
#define PIN_RED2 9
#define PIN_GREEN2 10
#define PIN_BLUE2 11
#define IR_PIN A0

int counter = 0;

// Number of colors used for animating, higher = smoother and slower animation)
int numColors = 255;

void setColor(unsigned char red, unsigned char green, unsigned char blue) {
  analogWrite(PIN_RED1, red);
  analogWrite(PIN_GREEN1, green);
  analogWrite(PIN_BLUE1, blue);

  analogWrite(PIN_RED2, green);
  analogWrite(PIN_GREEN2, blue);
  analogWrite(PIN_BLUE2, red);
}

long HSBtoRGB(float _hue, float _sat, float _brightness) {
  float red = 0.0;
  float green = 0.0;
  float blue = 0.0;

  if (_sat == 0.0) {
    red = _brightness;
    green = _brightness;
    blue = _brightness;
  } else {
    if (_hue == 360.0) {
      _hue = 0;
    }

    int slice = _hue / 60.0;
    float hue_frac = (_hue / 60.0) - slice;

    float aa = _brightness * (1.0 - _sat);
    float bb = _brightness * (1.0 - _sat * hue_frac);
    float cc = _brightness * (1.0 - _sat * (1.0 - hue_frac));

    switch (slice) {
      case 0:
        red = _brightness;
        green = cc;
        blue = aa;
        break;
      case 1:
        red = bb;
        green = _brightness;
        blue = aa;
        break;
      case 2:
        red = aa;
        green = _brightness;
        blue = cc;
        break;
      case 3:
        red = aa;
        green = bb;
        blue = _brightness;
        break;
      case 4:
        red = cc;
        green = aa;
        blue = _brightness;
        break;
      case 5:
        red = _brightness;
        green = aa;
        blue = bb;
        break;
      default:
        red = 0.0;
        green = 0.0;
        blue = 0.0;
        break;
    }
  }

  long ired = red * 255.0;
  long igreen = green * 255.0;
  long iblue = blue * 255.0;

  return long((ired << 16) | (igreen << 8) | (iblue));
}

// The combination of numColors and animationDelay determines the
// animation speed, I recommend a higher number of colors if you want
// to slow down the animation. Higher number of colors = smoother color
// changing.
int animationDelay =
    1;  // number milliseconds before RGB LED changes to next color

void setup() {
  // Serial.begin(115200);
  pinMode(PIN_RED1, OUTPUT);
  pinMode(PIN_BLUE1, OUTPUT);
  pinMode(PIN_GREEN1, OUTPUT);
  pinMode(PIN_RED2, OUTPUT);
  pinMode(PIN_BLUE2, OUTPUT);
  pinMode(PIN_GREEN2, OUTPUT);
  pinMode(IR_PIN, INPUT_PULLUP);
}

void loop() {
  const auto x = analogRead(IR_PIN);
  // Serial.println(x);
  // delay(100);
  if (x > 60) {
    setColor(0, 0, 0);
    return;
  };

  // This part takes care of displaying the
  // color changing in reverse by counting backwards if counter
  // is above the number of available colors
  float colorNumber = counter > numColors ? counter - numColors : counter;

  // Play with the saturation and brightness values
  // to see what they do
  float saturation = 1;    // Between 0 and 1 (0 = gray, 1 = full color)
  float brightness = .05;  // Between 0 and 1 (0 = dark, 1 is full brightness)
  float hue =
      (colorNumber / float(numColors)) * 360;  // Number between 0 and 360
  long color = HSBtoRGB(hue, saturation, brightness);

  // Get the red, blue and green parts from generated color
  int red = color >> 16 & 255;
  int green = color >> 8 & 255;
  int blue = color & 255;

  setColor(red, green, blue);

  // Counter can never be greater then 2 times the number of available colors
  // the colorNumber = line above takes care of counting backwards (nicely
  // looping animation) when counter is larger then the number of available
  // colors
  counter = (counter + 1) % (numColors * 2);

  // If you uncomment this line the color changing starts from the
  // beginning when it reaches the end (animation only plays forward)
  // counter = (counter + 1) % (numColors);

  delay(animationDelay);
}
