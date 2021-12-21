#include <FastLED.h>
#include <ImpulseList.h>

#define DATA_PIN   11
#define CLOCK_PIN  13
#define PIEZO_PIN  A0
#define BPM        98

// Information about the LED strip itself
#define NUM_LEDS    300
#define CHIPSET     APA102
#define COLOR_ORDER BGR
CRGB leds[NUM_LEDS];

// Setting this to a lower value may cause many "dim" lights to not show up at all
#define MAX_BRIGHTNESS  255

#define STANDARD_SPEED_PXPERSECOND 60
#define IMPULSE_LIFE_MS 10000

ImpulseList impulses;
unsigned long nextBeatTime = 0; // TODO: remove this when it's no longer used; exists to simulate received beats
unsigned long msPerBeat = static_cast<unsigned long>(60000 / BPM);

void setup() {
	Serial.begin(9600);
	Serial.println("DrumSaber Starting Up...");

	delay(1000); // power-up safety delay
	FastLED.addLeds<CHIPSET, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS);
	FastLED.setBrightness(MAX_BRIGHTNESS);
	FastLED.setMaxPowerInVoltsAndMilliamps(5, 2500);
}

// We do not expect people to be drumming faster than 16th beats
unsigned long minMSBetweenHits = static_cast<unsigned long>(1000 * 60 / (4.0f * BPM));

// min value to trigger a hit
unsigned long hitThreshold = 15;
unsigned long hitWindowMS = msPerBeat / 2; // MS before or after a beat to still be considered a "hit"
unsigned long nextAllowedHitMS = 0;
void receiveImpulses(unsigned long nowMS) {
	if (nowMS >= nextAllowedHitMS) {
		uint16_t piezoADC = analogRead(PIEZO_PIN);
		if (piezoADC > hitThreshold) {
			nextAllowedHitMS = nowMS + minMSBetweenHits;
			int offset = nowMS - nextBeatTime;
			if (offset < 0) offset = -offset; // TODO: replace this with better performant, but abs() wasn't working right
			// Serial.println("HIT! @ " + String(nowMS) + " value " + String(piezoADC) + " with offset " + String(offset) + " vs " + String(hitWindowMS));
			// if (offset <= hitWindowMS) {
				float power = 1.0f - static_cast<float>(offset) / hitWindowMS;
				impulses.add(nowMS, power, HIT);
			// }
		}
	}

	// Simulate beats being received and injected
	if (nowMS >= nextBeatTime) {
		impulses.add(nowMS, 0.1, BEAT);
		nextBeatTime += msPerBeat;
	}
}

// Age out impulses that have been alive too long
void manageImpulses(unsigned long nowMS) {
	// Make sure we don't wrap around and clear out everything
	if (nowMS < IMPULSE_LIFE_MS) return;
	int removed = impulses.purgeBefore(nowMS - IMPULSE_LIFE_MS);
	// if (removed > 0) {
        // Serial.println("...at " + String(nowMS) + " we removed " + String(removed) + " impulses; the collection should have " + String(impulses.count()) + " impulse(s).");
	// }
}

void renderBeat(Impulse* impulse, unsigned long nowMS) {
	unsigned long lifetime = nowMS - impulse->timestamp;
	uint16_t headLocation = static_cast<uint16_t>(lifetime * STANDARD_SPEED_PXPERSECOND / 1000.0f);
	if (headLocation < NUM_LEDS) {
		if (impulse->power > 0.5) {
			// major beat
			leds[headLocation].g += 15;
			leds[headLocation].b += 10;
		} else {
			leds[headLocation].b += 1;
		}
	}
}

void renderHit(Impulse* impulse, unsigned long nowMS) {
	uint16_t tailLength;
	float speed;
	uint8_t r, g, b;

	if (impulse->power > 0.7) {
		tailLength = 60;
		speed = STANDARD_SPEED_PXPERSECOND * 20.0f;
		r = 60;
		g = 10;
		b = 5;
	} else {
		tailLength = 10;
		speed = STANDARD_SPEED_PXPERSECOND * 5.0f;
		b = 0;
		if (impulse->power > 0.4) {
			r = 20;
			g = 15;
		} else {
			r = 5;
			g = 10;
		}
	}

	unsigned long lifetime = nowMS - impulse->timestamp;
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

void renderImpulses(unsigned long nowMS) {
	int renderCount = 0;

	Impulse* impulse = impulses.head;
	while (impulse != NULL) {
		if (impulse->type == BEAT) {
			renderBeat(impulse, nowMS);
		} else {
			renderHit(impulse, nowMS);
		}
		++renderCount;
		impulse = impulse->next;
	}
	// Serial.println("At "+String(nowMS)+" rendered "+String(renderCount)+"/"+String(impulses.count())+" impulse(s)");
}

void loop()
{
	unsigned long nowMS = millis();
	// Serial.println("frame @ " + String(nowMS));	
	receiveImpulses(nowMS);
	manageImpulses(nowMS);

	FastLED.clear();
	renderImpulses(nowMS);
	FastLED.show();
}
