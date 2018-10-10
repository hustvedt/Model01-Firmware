#include "Conway.h"


namespace kaleidoscope {

uint8_t Conway::board1[ROWS * 3 * COLS / 8];
uint8_t Conway::board2[ROWS * 3 * COLS / 8];
uint8_t *Conway::board = board1;
uint8_t *Conway::otherBoard = board2;

uint32_t Conway::next_heatmap_comp_time_ = 0;

uint16_t Conway::update_delay = 1000;

void Conway::resetMap(void) {
  board = board1;
  otherBoard = board2;
  // this method can be used as a way to work around an existing bug with a single key
  // getting special attention or if the user just wants a button to reset the map
  for (uint8_t r = 0; r < ROWS * 3; r++) {
    for (uint8_t c = 0; c < COLS; c++) {
      set_value_at(r, c, 0, true);
    }
  }
}


cRGB Conway::computeColor(uint8_t r, uint8_t g, uint8_t b) {
  return {r * 100, g * 100, b * 100};
}

EventHandlerResult Conway::onKeyswitchEvent(Key &mapped_key, byte row, byte col, uint8_t key_state) {
  // this methode is called frequently by Kaleidoscope
  // even if the module isn't activated

  // if it is a synthetic key, skip it
  if (key_state & INJECTED)
    return EventHandlerResult::OK;

  // if the key is not toggled on, skip it
  if (!keyToggledOn(key_state))
    return EventHandlerResult::OK;

  set_value_at(row * 3, col, 1, true);
  set_value_at(row * 3 + 1, col, 1, true);
  set_value_at(row * 3 + 2, col, 1, true);

  return EventHandlerResult::OK;
}


EventHandlerResult Conway::beforeEachCycle() {
  return EventHandlerResult::OK;
}

void Conway::update(void) {
  // this methode is called frequently by the LEDControl::loopHook

  // do nothing if we didn't reach next_heatmap_comp_time_ yet
  if (next_heatmap_comp_time_ && (millis() < next_heatmap_comp_time_))
    return;

  // schedule the next heatmap computing
  next_heatmap_comp_time_ = millis() + update_delay;

  // for each key
  for (uint8_t r = 0; r < ROWS; r++) {
    for (uint8_t c = 0; c < COLS; c++) {

      uint8_t red = get_value_at(r * 3, c);
      uint8_t green = get_value_at(r * 3 + 1, c);
      uint8_t blue = get_value_at(r * 3 + 2, c);

      // set the LED color accordingly
      ::LEDControl.setCrgbAt(r, c, computeColor(red, green, blue));
    }
  }

  stepBoard();
}


uint8_t Conway::get_value_at(uint8_t row, uint8_t column) {
  uint8_t bit_index = row * COLS + column;

  return (board[bit_index / 8] >> (bit_index % 8)) & 1;
}
uint8_t Conway::set_value_at(uint8_t row, uint8_t column, uint8_t value, bool front) {
  uint8_t bit_index = row * COLS + column;

  uint8_t *t;
  if (front)
    t = board;
  else
    t = otherBoard;
  uint8_t b = t[bit_index / 8];
  if (value == 1)
    bitSet(b, bit_index % 8);
  else
    bitClear(b, bit_index % 8);
  t[bit_index / 8] = b;

}
void Conway::stepBoard(void) {
  // Loop through every cell
  for (int8_t l = 0; l < ROWS * 3; l++)
  {
    for (int8_t m = 0; m < COLS; m++)
    {
      // finding no Of Neighbours that are alive
      uint8_t aliveNeighbours = 0;
      for (int8_t i = -1; i <= 1; i++) {
        for (int8_t j = -1; j <= 1; j++) {
          if (i != 0 || j != 0) {
            aliveNeighbours += get_value_at((l + i + ROWS * 3) % (ROWS * 3), (m + j + COLS) % COLS);
          }
        }
      }

      // Implementing the Rules of Life

      // Cell is lonely and dies
      uint8_t cell = get_value_at(l, m);
      if ((aliveNeighbours < 2) || (aliveNeighbours > 3)) {
        set_value_at(l, m, 0, false);
      }
      // A new cell is born
      else if ((cell == 0) && (aliveNeighbours == 3)) {
        set_value_at(l, m, 1, false);
      }
      // Remains the same
      else {
        set_value_at(l, m, cell, false);
      }
    }
  }

  uint8_t *temp = board;
  board = otherBoard;
  otherBoard = temp;
}
}

kaleidoscope::Conway ConwayEffect;
