#include <FastLED.h>
#include <vector>

class LedRenderer;
class SensorAnalyzer;

class BeatAnimator
{
public:
    BeatAnimator(SensorAnalyzer& sensors, LedRenderer& leds);
    void update(uint64_t time) &;

private:
    LedRenderer& m_leds;
    SensorAnalyzer& m_sensors;
    uint64_t m_lastStep;
};
