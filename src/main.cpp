#include <Arduino.h>
#include <FastLED.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include "pins.h"

static const int maxBrightness = 200;
static const int peeDelay = 10000;  // 1000 * 60 * 10;
static const int lightTrigger = 400;

static volatile bool lightUp = false;
static const int LED_COUNT = 29;
static CRGB leds[LED_COUNT];
static CRGB kelvinTable[] = {CRGB(0xff3800), CRGB(0xff4700), CRGB(0xff5300)};

void setColour() {
  for (int i = 0; i < LED_COUNT; i++) leds[i] = kelvinTable[1];
}

ISR(PCINT0_vect) { lightUp = true; }

void setup() {
  clock_prescale_set(clock_div_1);
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(LED_STIP_POWER_PIN, OUTPUT);
  GIMSK = _BV(PCIE);
  PCMSK = _BV(PIR_SENSOR_PIN);
  MCUCR |= (1 << ISC01) | (1 << ISC00);
  ADCSRA |= (1 << ADEN);
  sei();
  pinMode(3, OUTPUT);
  FastLED.addLeds<NEOPIXEL, LED_STRIP_PIN>(leds, LED_COUNT);
  setColour();
  FastLED.setBrightness(0);
  FastLED.show();
}

void sleep() {
  GIMSK = _BV(PCIE);
  PCMSK = _BV(PIR_SENSOR_PIN);
  ADCSRA &= ~_BV(ADEN);
  MCUCR |= _BV(ISC01) | _BV(ISC00);  // Added?
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  sleep_enable();
  sei();
  sleep_cpu();

  cli();
  PCMSK &= ~_BV(PIR_SENSOR_PIN);
  sleep_disable();
  ADCSRA |= _BV(ADEN);

  sei();
}

void loop() {
  digitalWrite(LED_STRIP_PIN, 1);
  digitalWrite(LED_STIP_POWER_PIN, 0);
  sleep();
  digitalWrite(LED_STRIP_PIN, 0);
  delay(100);
  const auto light = analogRead(A1);
  // FastLED.setBrightness(100);
  // for (int i = 0; i < LED_COUNT; i++)
  //   if (i * 37 < light)
  //     leds[i] = kelvinTable[1];
  //   else
  //     leds[i] = CRGB(0);
  // FastLED.show();
  // return;
  if (lightUp) {
    if (light < lightTrigger) {
      digitalWrite(LED_STIP_POWER_PIN, 1);
      delay(10);
      setColour();
      for (int i = 0; i < maxBrightness; i += 4) {
        FastLED.setBrightness(i);
        FastLED.show();
        delay(80);
      }
      delay(peeDelay);
      for (int i = maxBrightness; i >= 0; i--) {
        FastLED.setBrightness(i);
        FastLED.show();
        delay(200);
      }
    }
    lightUp = false;
    digitalWrite(LED_STIP_POWER_PIN, 0);
  }
}
