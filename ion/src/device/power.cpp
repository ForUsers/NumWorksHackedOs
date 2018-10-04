#include <ion.h>
#include "battery.h"
#include "device.h"
#include "display.h"
#include "keyboard.h"
#include "led.h"
#include "usb.h"
#include "wakeup.h"
#include "regs/regs.h"

void Ion::Power::suspend(bool checkIfPowerKeyReleased) {
  bool isLEDActive = Ion::LED::getColor() != KDColorBlack;
  if (checkIfPowerKeyReleased) {
    /* Wait until power is released to avoid restarting just after suspending */
    bool isPowerDown = true;
    while (isPowerDown) {
      Keyboard::State scan = Keyboard::scan();
      isPowerDown = scan.keyDown(Keyboard::Key::B2);
    }
  }
  Device::shutdownPeripherals(isLEDActive);

  PWR.CR()->setLPDS(true); // Turn the regulator off. Takes longer to wake up.
  PWR.CR()->setFPDS(true); // Put the flash to sleep. Takes longer to wake up.
  CM4.SCR()->setSLEEPDEEP(!isLEDActive);

  while (1) {
#if EPSILON_LED_WHILE_CHARGING
    /* Update LEDS
     * if the standby mode was stopped due to a "stop charging" event, we wait
     * a while to be sure that the plug state of the USB is up-to-date. */
    msleep(200);
    //Ion::LED::setCharging(Ion::USB::isPlugged(), Ion::Battery::isCharging());
#endif

    WakeUp::Device::onPowerKeyDown();
    WakeUp::Device::onUSBPlugging();
#if EPSILON_LED_WHILE_CHARGING
    WakeUp::Device::onChargingEvent();
#endif

    Device::shutdownClocks(isLEDActive);

   /* To enter sleep, we need to issue a WFE instruction, which waits for the
   * event flag to be set and then clears it. However, the event flag might
   * already be on. So the safest way to make sure we actually wait for a new
   * event is to force the event flag to on (SEV instruction), use a first WFE
   * to clear it, and then a second WFE to wait for a _new_ event. */
    asm("sev");
    asm("wfe");
    asm("nop");
    asm("wfe");

    Device::initClocks();

    Keyboard::Device::init();
    Keyboard::State scan = Keyboard::scan();
    Keyboard::Device::shutdown();

    Ion::Keyboard::State OnlyPowerKeyDown = Keyboard::State(Keyboard::Key::B2);
    if (scan == OnlyPowerKeyDown || USB::isPlugged()) {
      // Wake up
      break;
    }
  }
  Device::initClocks();

  Device::initPeripherals();
}
