#include "Arduino.h"
#include <TinyScreen.h>

#include "staio.h"

TinyScreen display = TinyScreen(TinyScreenPlus);

static uint16_t g_frameBuffer[96 * 64];

static void SendFrameBufferToDisplay() {
    display.startData();
    display.writeBuffer((uint8_t*)g_frameBuffer, sizeof(g_frameBuffer));
    display.endTransfer();
}

void setup() {
    // TinyScreen+
    display.begin();
    display.setBrightness(10); // 0-15
    display.setBitDepth(TSBitDepth16);

    // column start
    display.setX(0, 95);

    // row start
    display.setY(0, 63);

    // FS
    FILE *fp = fopen("fstest/smile.tfb", "rb"); // tfb = Tiny Frame Buffer
    if (!fp) {
        fprintf(stderr, "Could not open file\n");
        exit(1);
    }
    fread(g_frameBuffer, sizeof(uint16_t), 96 * 64, fp);
    fclose(fp);
}

void loop() {
    SendFrameBufferToDisplay();
}
