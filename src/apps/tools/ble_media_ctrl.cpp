/**
 * HW Abstraction class for BLE Media Device Controls
 * */

#include "./apps/tools/ble_media_ctrl.h"

#include <BleKeyboard.h>
#include <config.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

BleKeyboard* bleKeyboard;

/**
 * Initializes a BLE Keyboard instance and disables HAL's Display Buffer.
 * The BLE Keyboard is initialized with default values:
 *  @param deviceName = "osw"
 *  @param deviceManufacturer = "p3dt"
 *  @param batteryLevel = "100" (uint)
 * **/
void OswAppBLEMEdiaCtrl::setup(OswHal* hal) {
  hal->disableDisplayBuffer();
  bleKeyboard = new BleKeyboard("osw", "p3dt", 100);
  bleKeyboard->begin();
}

/**
 * Execution loop for Media Control logic ;
 * drawing of updates (4fps) on screen for device.
 * */
void OswAppBLEMEdiaCtrl::loop(OswHal* hal) {

  static long lastDraw = 0;
  static bool fillScreen = true;
  Serial.println(ESP.getFreeHeap());

 // Media Volume Controls (Raise=UP, Lower=DOWN)
  if (hal->btn3Down()) {
    bleKeyboard->write(KEY_MEDIA_VOLUME_UP);
  } else if (hal->btn2Down()) {
    bleKeyboard->write(KEY_MEDIA_VOLUME_DOWN);
  }

  //Only executes when current_millis - last draw time  > 4 fps
  if (millis() - lastDraw > 250 /* 4fps redraw */) {
    lastDraw = millis();

    // On first pass, Fill the screen with Black
    if (fillScreen) {
      fillScreen = false;
      hal->getCanvas()->getGraphics2D()->fill(rgb565(0, 0, 0));
    }

    // Set Graphics Canvas color to White (255,255,255)
    hal->getCanvas()->setTextColor(rgb565(255, 255, 255));

    // Sets Text Font Size to (2) (whatever this unit means ?)
    hal->getCanvas()->setTextSize(2);

    // Check connection state of BLE Device
    if (bleKeyboard->isConnected()) {

      //Toggles the state of Debounced buttons if any
      hal->getCanvas()->setCursor(100, 50);
      if (hal->btn3Down()) {
        hal->getCanvas()->print("Volume (+)");
      } else {
        hal->getCanvas()->print("Volume  + ");
      }
      hal->getCanvas()->setCursor(100, 190);
      if (hal->btn2Down()) {
        hal->getCanvas()->print("Volume (-)");
      } else {
        hal->getCanvas()->print("Volume  - ");
      }
    } else {
      hal->getCanvas()->setCursor(20, 120);
      hal->getCanvas()->print("Connecting...");
    }
    //Request the HAL to flush Associated BLE Device Buffers
    hal->requestFlush();
  }
}

/**
 * Finalizes the execution of the BLE Keyboard device, enables the DisplayBuffer and discards memory associated with BLE device.
 * */
void OswAppBLEMEdiaCtrl::stop(OswHal* hal) {
  bleKeyboard->end();
  delete bleKeyboard;
  hal->enableDisplayBuffer();
}