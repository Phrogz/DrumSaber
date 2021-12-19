#include <FastLED.h>
#include <ImpulseList.h>

#define DATA_PIN     11
#define CLOCK_PIN    13

// Information about the LED strip itself
#define NUM_LEDS    300
#define CHIPSET     APA102
#define COLOR_ORDER BGR
CRGB leds[NUM_LEDS];

// Setting this to a lower value may cause many "dim" lights to not show up at all
#define MAX_BRIGHTNESS  255

#define STANDARD_SPEED_PXPERSECOND 40
#define IMPULSE_LIFE_MS 10000

ImpulseList impulses;
unsigned long nextBeatTime = 0; // TODO: remove this when it's no longer used; exists to simulate received beats

void setup() {
	Serial.begin(9600);
	Serial.println("DrumSaber Starting Up...");

	delay(1000); // power-up safety delay
	FastLED.addLeds<CHIPSET, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS);
	FastLED.setBrightness(MAX_BRIGHTNESS);
	FastLED.setMaxPowerInVoltsAndMilliamps(5, 2500);
	// Serial.println("DrumSaber going now! @ " + String(nextBeatTime));
}

void receiveImpulses(unsigned long nowms) {
	if (random8() > 253) {
		float power = static_cast<float>(static_cast<float>(random16()) / (0xFFFF * 1.0f));
		impulses.add(nowms, power, HIT);
		// Serial.println("...injected hit at " + String(nowms) + " with power " + String(power) + " for a total of "+String(impulses.count())+" impulse(s)");
	}

	// Simulate beats being received and injected
	if (nowms >= nextBeatTime) {
		bool isMajorBeat = (nextBeatTime % 2000 == 0);
		impulses.add(nowms, isMajorBeat ? 1.0 : 0.1, BEAT);
		nextBeatTime += 500;
		// Serial.println("...injected beat at " + String(nowms) + " for a total of "+String(impulses.count())+" impulse(s)");
	}
}

// Age out impulses that have been alive too long
void manageImpulses(unsigned long nowms) {
	// Make sure we don't wrap around and clear out everything
	if (nowms < IMPULSE_LIFE_MS) return;
	int removed = impulses.purgeBefore(nowms - IMPULSE_LIFE_MS);
	if (removed > 0) {
        // Serial.println("...at " + String(nowms) + " we removed " + String(removed) + " impulses; the collection should have " + String(impulses.count()) + " impulse(s).");
	}
}

void renderBeat(Impulse* impulse, unsigned long nowms) {
	unsigned long lifetime = nowms - impulse->timestamp;
	uint16_t headLocation = static_cast<uint16_t>(lifetime * STANDARD_SPEED_PXPERSECOND / 1000.0f);
	if (headLocation < NUM_LEDS) {
		if (impulse->power > 0.5) {
			// major beat
			leds[headLocation].g += 15;
			leds[headLocation].b += 10;
			Serial.println('MAJOR!');
		} else {
			leds[headLocation].b += 1;
		}
	}
}

void renderHit(Impulse* impulse, unsigned long nowms) {
	uint16_t tailLength;
	float speed;
	uint8_t r, g, b;

	if (impulse->power > 0.7) {
		tailLength = 20;
		speed = STANDARD_SPEED_PXPERSECOND * 2.5f;
		r = 30;
		g = 5;
		b = 3;
	} else {
		tailLength = 10;
		speed = STANDARD_SPEED_PXPERSECOND * 1.5f;
		b = 0;
		if (impulse->power > 0.4) {
			r = 20;
			g = 15;
		} else {
			r = 5;
			g = 10;
		}
	}

	unsigned long lifetime = nowms - impulse->timestamp;
	uint16_t headLocation = static_cast<uint16_t>(lifetime * speed / 1000.0f);
	for (uint16_t offset=0; offset<tailLength; ++offset) {
		int index = headLocation - offset;
		if (index >= 0 && index < NUM_LEDS) {
			float falloff = (1.0 - (offset*1.0f)/(tailLength*1.0f)) * impulse->power;
			leds[index].r += r * falloff;
			leds[index].g += g * falloff;
			leds[index].b += b * falloff;
		}
	}
}

void renderImpulses(unsigned long nowms) {
	int renderCount = 0;

	Impulse* impulse = impulses.head;
	while (impulse != NULL) {
		if (impulse->type == BEAT) {
			renderBeat(impulse, nowms);
		} else {
			renderHit(impulse, nowms);
		}
		++renderCount;
		impulse = impulse->next;
	}
	// Serial.println("At "+String(nowms)+" rendered "+String(renderCount)+"/"+String(impulses.count())+" impulse(s)");
}

void loop()
{
	unsigned long nowms = millis();
	// Serial.println("frame @ " + String(nowms));	
	receiveImpulses(nowms);
	manageImpulses(nowms);

	FastLED.clear();
	renderImpulses(nowms);
	FastLED.show();
}
