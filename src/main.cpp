#include <Arduino.h>
#include <SparkFun_MiniGen.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SparkFun_Qwiic_Twist_Arduino_Library.h>

#define WIRE Wire

MiniGen gen;
TWIST twist;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &WIRE);

#define START_FREQUENCY 0.0

#define SAMPLES 24

// state machine for settings
enum StateMachine {
    MODE,
    FREQUENCY,
    INCREMENT
};

// put function declarations here:
void updateDisplay(float, int, float, int);
void graphSine();
void graphSquare();
void graphSquare_2();
void graphTriangle();
void drawPip(int, int);

void setup() {
    Serial.begin(9600);
    twist.begin();
    twist.setCount(0); // reset the count
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

    Serial.println("Starting up the waveform generator...");
    gen.reset();
    gen.setMode(MiniGen::SINE);
    gen.setFreqAdjustMode(MiniGen::FULL);
    uint32_t newFrequency = gen.freqCalc(START_FREQUENCY);
    gen.adjustFreq(MiniGen::FREQ0, newFrequency);
    Serial.println("Waveform generator is ready");

    display.clearDisplay();
    display.display();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    updateDisplay(newFrequency, MiniGen::FULL, 100.0, FREQUENCY);
    display.display();
}

void loop() {
    static int stateMachine = FREQUENCY;
    static int currentMode = MiniGen::SINE;
    static float frequency = START_FREQUENCY;
    static float increment = 100.0;
    static int last = 0;

    int16_t moved = twist.getCount();
    boolean clicked = twist.isClicked();

    if (clicked) {
        int currentState = stateMachine;
        switch (currentState) {
            case FREQUENCY:
                stateMachine = MODE;
                break;
            case MODE:
                stateMachine = INCREMENT;
                break;
            case INCREMENT:
                stateMachine = FREQUENCY;
                break;
            default:
                stateMachine = FREQUENCY;
                break;
        }
        updateDisplay(frequency, currentMode, increment, stateMachine);
    }

    if (moved != last) {
        uint16_t diff = twist.getDiff();
        if (stateMachine == FREQUENCY) {
            if (diff == 1) {
                frequency += increment;
            } else {
                if (frequency > START_FREQUENCY) {
                    frequency -= increment;
                }
            }
            uint32_t newFrequency = gen.freqCalc(frequency);
            gen.adjustFreq(MiniGen::FREQ0, newFrequency);
        }

        if (stateMachine == MODE) {
            if (diff == 1) {
                switch(currentMode) {
                    case MiniGen::SINE:
                        gen.setMode(MiniGen::TRIANGLE);
                        currentMode = MiniGen::TRIANGLE;
                        break;
                    case MiniGen::TRIANGLE:
                        gen.setMode(MiniGen::SQUARE);
                        currentMode = MiniGen::SQUARE;
                        break;
                    case MiniGen::SQUARE:
                        gen.setMode(MiniGen::SQUARE_2);
                        currentMode = MiniGen::SQUARE_2;
                        break;
                    case MiniGen::SQUARE_2:
                        gen.setMode(MiniGen::SINE);
                        currentMode = MiniGen::SINE;
                        break;
                    default:
                        gen.setMode(MiniGen::SINE);
                        break;
                }
            } else {
                switch(currentMode) {
                    case MiniGen::SINE:
                        gen.setMode(MiniGen::SQUARE_2);
                        currentMode = MiniGen::SQUARE_2;
                        break;
                    case MiniGen::SQUARE_2:
                        gen.setMode(MiniGen::SQUARE);
                        currentMode = MiniGen::SQUARE;
                        break;
                    case MiniGen::SQUARE:
                        gen.setMode(MiniGen::TRIANGLE);
                        currentMode = MiniGen::TRIANGLE;
                        break;
                    case MiniGen::TRIANGLE:
                        gen.setMode(MiniGen::SINE);
                        currentMode = MiniGen::SINE;
                        break;
                    default:
                        gen.setMode(MiniGen::SINE);
                        break;
                }
            }
        }

        if (stateMachine == INCREMENT) {
            float currentIncrement = increment;

            if (diff == 1) {
                if (currentIncrement == 0.1) { increment = 1.0; }
                if (currentIncrement == 1.0) { increment = 10.0; }
                if (currentIncrement == 10.0) { increment = 100.0; }
                if (currentIncrement == 100.0) { increment = 1000.0; }
                if (currentIncrement == 1000.0) { increment = 10000.0; }
                if (currentIncrement == 10000.0) { increment = 100000.0; }
                if (currentIncrement == 100000.0) { increment = 0.1; }
            } else {
                if (currentIncrement == 0.1) { increment = 100000.0; }
                if (currentIncrement == 1.0) { increment = 0.1; }
                if (currentIncrement == 10.0) { increment = 1.0; }
                if (currentIncrement == 100.0) { increment = 10.0; }
                if (currentIncrement == 1000.0) { increment = 100.0; }
                if (currentIncrement == 10000.0) { increment = 1000.0; }
                if (currentIncrement == 100000.0) { increment = 10000.0; }
            }
        }

        last = moved;
        updateDisplay(frequency, currentMode, increment, stateMachine);
    }
    yield();
}

// put function definitions here:
void updateDisplay(float freq, int mode, float increment, int state) {
    String hz;
    String modeName;
    float div;
    int pip = 1;

    if (freq >= 1000000.0) {
        hz = "mHz";
        div = 1000000;
    } else if (freq >= 1000.0) {
        hz = "kHz";
        div = 1000.0;
    } else {
        hz = "Hz";
        div = 1.0;
    }

    switch (mode) {
        case MiniGen::SINE:
            modeName = "sine";
            break;
        case MiniGen::TRIANGLE:
            modeName = "tri";
            break;
        case MiniGen::SQUARE:
            modeName = "sq";
            break;
        case MiniGen::SQUARE_2:
            modeName = "sq/2";
            break;
        default:
            modeName = "";
            break;
    }

    switch (state) {
        case FREQUENCY:
            pip = 1;
            break;
        case MODE:
            pip = 12;
            break;
        case INCREMENT:
            pip = 24;
            break;
        default:
            break;
    }
    display.clearDisplay();
    drawPip(1,pip);
    display.setCursor(16 ,0);
    display.print("Freq: ");
    display.print(freq/div);
    display.print(hz);
    display.setCursor(16 ,12);
    display.print("Wave: ");
    display.print(modeName);
    display.setCursor(16 ,24);
    display.print("Inc: ");
    display.print(increment);
    display.display();
}

void graphSine() {
    for (int i = 0; i < SAMPLES; i++) {
        float v = sin(2.0 * PI * i / SAMPLES);
        display.drawPixel(i, map(v * 10, -10.0, 10.0, 0.0, 16.0), SSD1306_WHITE);
    }
}

void graphSquare() {
    display.drawFastVLine(0, 0, 8, SSD1306_WHITE);
    display.drawFastVLine(6, 0, 16, SSD1306_WHITE);
    display.drawFastVLine(12, 0, 16, SSD1306_WHITE);
    display.drawFastVLine(18, 0, 16, SSD1306_WHITE);
    display.drawFastVLine(24, 8, 8, SSD1306_WHITE);
    display.drawFastHLine(0, 0, 6, SSD1306_WHITE);
    display.drawFastHLine(6, 16, 7, SSD1306_WHITE);
    display.drawFastHLine(12, 0, 6, SSD1306_WHITE);
    display.drawFastHLine(18, 16, 7, SSD1306_WHITE);
}

void graphSquare_2() {
    display.drawFastVLine(0, 0, 8, SSD1306_WHITE);
    display.drawFastVLine(12, 0, 16, SSD1306_WHITE);
    display.drawFastVLine(24, 8, 8, SSD1306_WHITE);
    display.drawFastHLine(0, 0, 12, SSD1306_WHITE);
    display.drawFastHLine(12, 16, 13, SSD1306_WHITE);
}

void graphTriangle() {
    display.drawLine(0, 8, 4, 0, SSD1306_WHITE);
    display.drawLine(4, 0, 12, 16, SSD1306_WHITE);
    display.drawLine(12, 16, 20, 0, SSD1306_WHITE);
    display.drawLine(20, 0, 24, 8, SSD1306_WHITE);
}

void drawPip(int x, int y) {
    int radius = 2;
    int padding = 1;
    display.fillCircle(x + radius + padding, y + radius + padding, radius * 2, SSD1306_WHITE);
}