
#pragma once

#include <Kaleidoscope.h>
#include <Kaleidoscope-LEDControl.h>

namespace kaleidoscope {
class Conway : public LEDMode {
 public:
  Conway(void) {}

  void resetMap(void);

  EventHandlerResult onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t key_state);
  EventHandlerResult beforeEachCycle();

  static uint16_t update_delay;
 protected:
  void update(void) final;

 private:
  static uint32_t next_heatmap_comp_time_;
  static uint8_t board1[ROWS * 3][COLS];
  static uint8_t board2[ROWS * 3][COLS];
  static uint8_t (*board)[COLS];
  static uint8_t (*otherBoard)[COLS];
  static cRGB computeColor(uint8_t r, uint8_t g, uint8_t b);
  static void stepBoard(void);
};
}

extern kaleidoscope::Conway ConwayEffect;
