#include <FastLED.h>
#include <ImpulseList.h>

#define DATA_PIN     11
#define CLOCK_PIN    13

// Information about the LED strip itself
#define NUM_LEDS    300
#define CHIPSET     APA102
#define COLOR_ORDER BGR
CRGB leds[NUM_LEDS];

#define MAX_BRIGHTNESS  255

#define MAX_IMPULSE_BRIGHTNESS 20
#define MAX_IMPULSE_TAILLENGTH_PX 25
#define MAX_IMPULSE_SPEED_PXPERSECOND 60
#define IMPULSE_LIFE_MS 15000

ImpulseList impulses;

void setup() {
	delay(2000); // power-up safety delay
	FastLED.addLeds<CHIPSET, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS);
	FastLED.setBrightness(MAX_BRIGHTNESS);
	FastLED.setMaxPowerInVoltsAndMilliamps(5, 2500);

	Serial.begin(9600);
	Serial.println("DrumSaber Starting Up...");
}

void receiveImpulses(unsigned long now) {
	if (random8() > 252) {
		float power = static_cast<float>(static_cast<float>(random16()) / (0xFFFF * 1.0f));
		impulses.add(now, power);
		Serial.println("...injecting impulse at " + String(now) + " with power " + String(power));
	}
}

// Age out any impulses that have bee alive too long.
void manageImpulses(unsigned long nowms) {
	// TODO: move this into a class method that expires all impulses with a timestamp before a specified time
	unsigned long expireTime = nowms - IMPULSE_LIFE_MS;
	Impulse* impulse = impulses.head;
	while (impulse != NULL) {
		if (impulse->timestamp <= expireTime) {
			Serial.println("...removing impulse from " + impulse->timestamp);
			impulses.remove(impulse->timestamp);
		}
		impulse = impulse->next;
	}
}

void renderImpulses(unsigned long nowms) {
	Impulse* impulse = impulses.head;
	while (impulse != NULL) {
		uint16_t tailLength = static_cast<uint16_t>(MAX_IMPULSE_TAILLENGTH_PX * impulse->power);
		float brightness = MAX_IMPULSE_BRIGHTNESS * impulse->power;
		unsigned long lifetime = nowms - impulse->timestamp;
		uint16_t headLocation = static_cast<uint16_t>(lifetime * MAX_IMPULSE_SPEED_PXPERSECOND / 1000.0f * impulse->power);
		for (uint16_t offset=0; offset<tailLength; ++offset) {
			int index = headLocation - offset;
			if (index >= 0 && index < NUM_LEDS) {
				uint8_t cellBrightness = static_cast<uint8_t>(brightness * (1.0 - (offset*1.0f)/(tailLength*1.0f)));
				leds[index].red += cellBrightness;
				leds[index].green += (cellBrightness >> 1);
			}
		}

		impulse = impulse->next;
	}
}

void loop()
{
	unsigned long nowms = millis();
	// Serial.println("...loop @ " + String(nowms));
	receiveImpulses(nowms);
	manageImpulses(nowms);

	FastLED.clear();
	renderImpulses(nowms);
	FastLED.show();
}
