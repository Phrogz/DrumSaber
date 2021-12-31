#include <FastLED.h>
#include <vector>

class LedRenderer
{
public:
    LedRenderer() :
        m_status( CRGB::Black ),
        m_leds( kLedCount )
    {
        FastLED.addLeds<APA102, kDataPin, kClockPin, BGR>(m_leds.data(), m_leds.size());
        FastLED.addLeds<SK6812, kStatusPin, GRB>(&m_status, 1);
        FastLED.clear();
    }

    void setStatus(CRGB const& color) & { m_status = color; }
    std::vector<CRGB>& getLeds() & { return m_leds; }
    void show() const& { FastLED.show(); }

private:
    CRGB m_status;
    std::vector<CRGB> m_leds;
};
