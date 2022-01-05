#include <exception>
#include <mutex>
#include <vector>

class SensorAnalyzer
{
public:
    SensorAnalyzer()
    {
        pinMode(kButtonPin, INPUT);
        if (!adcAttachPin(kPiezoPin))
            log_e("Could not initialize ADC");
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
            log_d("Button down %llu ms", time - m_lastButtonTime);

            std::lock_guard<std::mutex> guard(m_mutex);
            m_beats.push_back({time, static_cast<uint8_t>(std::min(time - m_lastButtonTime, 1000ULL) * 255ULL / 1000ULL)});
        }
        m_lastButtonState = state;

        // Piezo test
        uint16_t piezo{ analogRead(kPiezoPin) };
        if (piezo != 0)
        {
            log_d("kPiezoPin analogRead: %u", piezo);

            std::lock_guard<std::mutex> guard(m_mutex);
            m_beats.push_back({time, static_cast<uint8_t>(std::min<uint64_t>(piezo, 1000) * 255ULL / 1000ULL)});
        }
    }

    bool hasBeats() &
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        return !m_beats.empty();
    }

    struct Beat
    {
        uint64_t time;
        uint8_t intensity;
    };

    /**
     * Return all the recent beats since last call and clear the beats vector.
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
