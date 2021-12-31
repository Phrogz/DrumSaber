#include "BeatAnimator.hpp"

#include "SensorAnalyzer.hpp"
#include "LedRenderer.hpp"

 BeatAnimator::BeatAnimator(SensorAnalyzer& sensors, LedRenderer& leds) :
    m_leds{leds},
    m_sensors{sensors},
    m_lastStep{}
{
}

void BeatAnimator::update(uint64_t time) &
{
    CRGB* leds{m_leds.getLeds().data()};
    if (time > m_lastStep + 20)
    {
        memmove8(leds+1, leds, (m_leds.getLeds().size() - 1) * sizeof(CRGB));
        *leds = CRGB::Black;
        m_lastStep = time;
    }

    for (auto beat : m_sensors.getBeats())
    {
        Serial.printf("Button down time:%.2f intensity:%d\n", beat.time / 1000.0F, beat.intensity);
        *leds = CHSV(beat.intensity, 255, 255);
    }
}
