#include <FastLED.h>
#include "SensorAnalyzer.hpp"
#include "BeatAnimator.hpp"
#include "LedRenderer.hpp"

SensorAnalyzer* sensors{};
BeatAnimator* animator{};
LedRenderer* leds{};

void setup()
{
    Serial.begin(115200);
	Serial.print("DrumSaber...");

    FastLED.setMaxPowerInVoltsAndMilliamps(5, 200);

    sensors = new SensorAnalyzer();
    leds = new LedRenderer();
    animator = new BeatAnimator(*sensors, *leds);

    Serial.println(" started,");
}

void loop()
{
    // Blink status LED every second
    uint64_t time = millis();
    leds->setStatus(time % 1000 < 10 ? CRGB::DarkBlue : CRGB::Black);
//  fill_rainbow(leds->getLeds().data(), leds->getLeds().size(), (time / 5) % 256, 1); // Debug rainbow

    sensors->read(time);
    animator->update(time);
    leds->show();
}