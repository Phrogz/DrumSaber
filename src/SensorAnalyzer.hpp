#include <vector>
#include <mutex>

class SensorAnalyzer
{
public:
    SensorAnalyzer()
    {
        pinMode(kButtonPin, INPUT);
        //if (!adcAttachPin(kPiezoPin)) // non-blocking todo
    }

    /**
     * Read all sensors and translate them to beats.
     *
     * @note Intended to be called in a separate thread, possibly on a dedicated cpu core.
     */
    void read(uint64_t time) &
    {
        // Button is debug beat producer
        int state{ digitalRead(kButtonPin) };
        if (m_lastButtonState == HIGH && state == LOW)
        {
            m_lastButtonTime = time;
        }
        else if (m_lastButtonState == LOW && state == HIGH)
        {
            Serial.printf("Button down %llu ms\n", time - m_lastButtonTime);

            std::lock_guard<std::mutex> guard(m_mutex);
            m_beats.push_back({time, static_cast<uint8_t>(std::min(time - m_lastButtonTime, 1000ULL) * 255ULL / 1000ULL)});
        }
        m_lastButtonState = state;

        // Piezo test
        uint16_t piezo{ analogRead(kPiezoPin) };
        if (piezo != 0)
        {
            Serial.printf("kPiezoPin: %u\n", piezo);

            std::lock_guard<std::mutex> guard(m_mutex);
            m_beats.push_back({time, static_cast<uint8_t>(std::min<uint64_t>(piezo, 1000) * 255ULL / 1000ULL)});
        }
    }

    struct Beat
    {
        uint64_t time;
        uint8_t intensity;
    };

    /**
     * Return all the recent beats since last call.
     */
    std::vector<Beat> getBeats() &
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        std::vector<Beat> newBeats{ std::move(m_beats) };
        return newBeats;
    };

private:
    std::mutex m_mutex;
    std::vector<Beat> m_beats;

    int m_lastButtonState{HIGH};
    uint64_t m_lastButtonTime{0};
};
