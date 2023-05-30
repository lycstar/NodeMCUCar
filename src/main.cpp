
#include "ESP8266WiFi.h"
#include "WebSocketsClient.h"
#include "Adafruit_SSD1306.h"
#include "WebSocketsServer.h"


const char *ssid = "Car";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET     (-1)

#define SCREEN_ADDRESS 0x3C


#define EN1 D1
#define EN2 D3
#define EN3 D5
#define EN4 D7

#define DIR1_PIN D1
#define PWM1_PIN D2
#define DIR2_PIN D3
#define PWM2_PIN D4
#define DIR3_PIN D5
#define PWM3_PIN D6
#define DIR4_PIN D7
#define PWM4_PIN D8

#define PWM_PRECISION 10000

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WebSocketsServer webSocket = WebSocketsServer(8080);;


void drawText(const String &text) {
//    int16_t x1;
//    int16_t y1;
//    uint16_t width;
//    uint16_t height;
//    display.clearDisplay();
//    display.setTextSize(1);      // Normal 1:1 pixel scale
//    display.setTextColor(SSD1306_WHITE); // Draw white text
//    display.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);
//    display.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2);
//    display.cp437(true);
//    display.println(text);
//    display.display();
    Serial.println(text);
}


//正转
void forwardRun() {
    digitalWrite(DIR1_PIN, HIGH);
    digitalWrite(PWM1_PIN, LOW);

    digitalWrite(DIR2_PIN, HIGH);
    digitalWrite(PWM2_PIN, LOW);
//
    digitalWrite(DIR3_PIN, HIGH);
    digitalWrite(PWM3_PIN, LOW);
//
    digitalWrite(DIR4_PIN, HIGH);
    digitalWrite(PWM4_PIN, LOW);
}


//反转
void backwardRun() {
    digitalWrite(DIR1_PIN, LOW);
    digitalWrite(PWM1_PIN, HIGH);

    digitalWrite(DIR2_PIN, LOW);
    digitalWrite(PWM2_PIN, HIGH);

    digitalWrite(DIR3_PIN, LOW);
    digitalWrite(PWM3_PIN, HIGH);

    digitalWrite(DIR4_PIN, LOW);
    digitalWrite(PWM4_PIN, HIGH);
}


//停止
void stop() {
    digitalWrite(DIR1_PIN, LOW);
    digitalWrite(PWM1_PIN, LOW);

    digitalWrite(DIR2_PIN, LOW);
    digitalWrite(PWM2_PIN, LOW);

    digitalWrite(DIR3_PIN, LOW);
    digitalWrite(PWM3_PIN, LOW);

    digitalWrite(DIR4_PIN, LOW);
    digitalWrite(PWM4_PIN, LOW);
}


void leftTurn() {
    stop();
    analogWrite(DIR2_PIN, PWM_PRECISION);
    analogWrite(DIR4_PIN, PWM_PRECISION);
}

void rightTurn() {
    stop();
    analogWrite(DIR1_PIN, PWM_PRECISION);
    analogWrite(DIR3_PIN, PWM_PRECISION);
}


void runCommand(char *command) {

    if (strcmp(command, "stop") == 0) {
        stop();
    }

    if (strcmp(command, "forward") == 0) {
        forwardRun();
    }

    if (strcmp(command, "backward") == 0) {
        backwardRun();
    }

    if (strcmp(command, "left") == 0) {
        leftTurn();
    }

    if (strcmp(command, "right") == 0) {
        rightTurn();
    }


}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED: {
            drawText("Connected!");
            webSocket.sendTXT(num, "Connected!");
        }
            break;
        case WStype_DISCONNECTED: {
            drawText("Waiting!");
        }
            break;
        case WStype_TEXT: {
            runCommand((char *) payload);
            drawText((char *) payload);
        }
            break;

    }
}


void setupSocket() {
    webSocket.begin();
    drawText("Waiting!");
    webSocket.onEvent(webSocketEvent);
}


void setupWifi() {
    WiFi.softAP(ssid);
    drawText("Wifi Setup.");
    delay(1000);
}

void setupOLED() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }
    display.display();
    delay(1000);
}


void setupEM() {
    // 设置引脚模式
    pinMode(DIR1_PIN, OUTPUT);
    pinMode(PWM1_PIN, OUTPUT);

    pinMode(DIR2_PIN, OUTPUT);
    pinMode(PWM2_PIN, OUTPUT);

    pinMode(DIR3_PIN, OUTPUT);
    pinMode(PWM3_PIN, OUTPUT);

    pinMode(DIR4_PIN, OUTPUT);
    pinMode(PWM4_PIN, OUTPUT);
}


void setup() {
    Serial.begin(9600);
    setupWifi();
    setupEM();
    setupSocket();
}


void loop() {
    webSocket.loop();
}
