#include "display.h"
#include "getImage.h"
#include "util.h"

#include <TFT_eSPI.h>
// A library for interfacing with LCD displays
//
// Can be installed from the library manager (Search for "TFT_eSPI")
// https://github.com/Bodmer/TFT_eSPI

#include <PNGdec.h>
// For decoding png files
//
// Can be installed from the library manager (Search for "PNGdec")
// https://github.com/bitbank2/PNGdec

// -------------------------------
// Putting this stuff outside the class because
// I can't easily pass member functions in as callbacks for pngdec

#include <SPI.h>
#include <XPT2046_Touchscreen_TT.h>

#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

SPIClass touchSpi = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

// -------------------------------

#define SESSION_TEXT_SIZE 4

TFT_eSPI tft = TFT_eSPI();
PNG png;

fs::File myfile;

void *myOpen(const char *filename, int32_t *size) {
  myfile = SPIFFS.open(filename);
  *size = myfile.size();
  return &myfile;
}
void myClose(void *handle) {
  if (myfile)
    myfile.close();
}
int32_t myRead(PNGFILE *handle, uint8_t *buffer, int32_t length) {
  if (!myfile)
    return 0;
  return myfile.read(buffer, length);
}
int32_t mySeek(PNGFILE *handle, int32_t position) {
  if (!myfile)
    return 0;
  return myfile.seek(position);
}

int PNGDraw(PNGDRAW *pDraw) {
  static uint16_t usPixels[320];

  if (pDraw->iWidth > 320)
    return 0;

  // Center horizontally if the PNG line is narrower than the screen
  int x = (320 - pDraw->iWidth) / 2;

  // Use BIG_ENDIAN (seemed mostly ok for user before) and blend against black
  // background
  png.getLineAsRGB565(pDraw, usPixels, PNG_RGB565_BIG_ENDIAN, 0x00000000);
  tft.pushImage(x, pDraw->y, pDraw->iWidth, 1, usPixels);
  return 1;
}

class CheapYellowDisplay : public F1Display {
public:
  void displaySetup() {

    Serial.println("cyd display setup");
    setWidth(320);
    setHeight(240);

    // Start the tft display and set it to black
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // Start the touch screen
    touchSpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    ts.begin(touchSpi);
    ts.setRotation(1);

    state = unset;
  }

  void displayPlaceHolder(const char *raceName, JsonObject races_sessions) {

    if (!isSameRace(raceName) || state != placeholder) {
      setRaceName(raceName);
      TrackAsset asset = getTrackAssetForRace(raceName);
      if (asset.data) {
        int imageDisplayStatus = displayImage(asset.data, asset.length);
        if (imageDisplayStatus == PNG_SUCCESS) {
          // Image is displayed
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          int yPos = 215;
          String gpStartDateStr =
              String(getConvertedTime(races_sessions["gp"], "M d"));
          String displayMessage =
              String(convertRaceName(raceName)) + " | " + gpStartDateStr;
          tft.drawCentreString(displayMessage, screenCenterX, yPos, 4);
          state = placeholder;
          return;
        }
      }
      // Failed to display the image
      displayRaceWeek(raceName, races_sessions); // For now
    }

    // if we reach here, the screen doesn't need to be updated
    Serial.println("No need to update display");
  }

  void displayRaceWeek(const char *raceName, JsonObject races_sessions) {
    Serial.println("prts");
    tft.fillRect(0, 0, screenWidth, screenHeight, TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    int yPos = 5;
    String gpStartDateStr =
        String(getConvertedTime(races_sessions["gp"], "M d"));
    String displayMessage =
        String(convertRaceName(raceName)) + " | " + gpStartDateStr;
    tft.drawCentreString(displayMessage, screenCenterX, yPos, 4);

    int yValue = 46;
    for (JsonPair kv : races_sessions) {
      printSession(4, yValue, sessionCodeToString(kv.key().c_str()),
                   getConvertedTime(kv.value().as<const char *>()));
      yValue += (SESSION_TEXT_SIZE) * 8;
    }

    state = raceweek;
  }

  int displayImage(const uint8_t *data, int len) {
    tft.fillScreen(TFT_BLACK);
    unsigned long lTime = millis();
    Serial.println("Displaying PROGMEM image");

    int rc = png.openRAM((uint8_t *)data, len, PNGDraw);
    if (rc == PNG_SUCCESS) {
      Serial.printf("PNG specs: (%d x %d), %d bpp, pixel type: %d, size: %d\n",
                    png.getWidth(), png.getHeight(), png.getBpp(),
                    png.getPixelType(), len);
      rc = png.decode(NULL, 0);
      Serial.printf("PNG decode result: %d\n", rc);
      png.close();
    } else {
      Serial.print("PNG open error: ");
      Serial.println(rc);
    }

    Serial.print("Time taken (ms): ");
    Serial.println(millis() - lTime);

    return rc;
  }

  void drawWifiManagerMessage(WiFiManager *myWiFiManager) {
    Serial.println("Entered Conf Mode");
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("Entered Conf Mode:", screenCenterX, 5, 2);
    tft.drawString("Connect to the following WIFI AP:", 5, 28, 2);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.drawString(myWiFiManager->getConfigPortalSSID(), 20, 48, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Password:", 5, 64, 2);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.drawString("nomikey1", 20, 82, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.drawString("If it doesn't AutoConnect, use this IP:", 5, 110, 2);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.drawString(WiFi.softAPIP().toString(), 20, 128, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  }

  unsigned long lastTouchTime = 0;

  TouchAction checkTouch() override {
    if (ts.touched() && millis() - lastTouchTime > 300) {
      lastTouchTime = millis();
      TS_Point p = ts.getPoint();

      // Simple scaling from raw touch coordinates to screen pixels
      // Note: XPT2046 raw values for 320x240 rotation 1 usually range ~200 to
      // 3800
      int x = map(p.x, 200, 3800, 1, 320);
      int y = map(p.y, 250, 3800, 1, 240);

      if (state == placeholder) {
        // Image view: Top of screen is image, bottom is text
        // Text is at bottom (Y > 200)
        if (y > 200) {
          if (x < screenCenterX)
            return TOUCH_PREV; // Left side: track/gp name
          if (x >= screenCenterX)
            return TOUCH_NEXT; // Right side: date
        } else {
          return TOUCH_TOGGLE; // Top section toggles layout
        }
      } else if (state == raceweek) {
        // Session view: Top of screen is text (Y < 40), bottom is sessions
        if (y < 40) {
          if (x < screenCenterX)
            return TOUCH_PREV; // Left side: track/gp name
          if (x >= screenCenterX)
            return TOUCH_NEXT; // Right side: date
        } else {
          return TOUCH_TOGGLE; // Rest of session toggles layout
        }
      }
    }
    return TOUCH_NONE;
  }

private:
  void printSession(int x, int y, const char *sessionName,
                    String sessionStartTime) {
    String tempStr = String(sessionName);
    tempStr += " ";
    tempStr += sessionStartTime;
    tft.drawString(tempStr, x, y, SESSION_TEXT_SIZE);
  }
};
