#include "BeatAnimator.hpp"

#include "LedRenderer.hpp"
#include "SensorAnalyzer.hpp"

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

    if (m_sensors.hasBeats())
    {
        auto const beats{ m_sensors.getBeats() };
        log_d("Beats %d:", beats.size());
        for (auto beat : beats)
        {
            log_d("  time:%.2f intensity:%d", beat.time / 1000.0F, beat.intensity);
            *leds = CHSV(beat.intensity, 255, 255);
        }
    }
}
