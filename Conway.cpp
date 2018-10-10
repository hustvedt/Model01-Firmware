#include "Conway.h"


namespace kaleidoscope {

uint8_t Conway::board1[ROWS * 3][COLS];
uint8_t Conway::board2[ROWS * 3][COLS];
uint8_t (*Conway::board)[COLS] = board1;
uint8_t (*Conway::otherBoard)[COLS] = board2;

uint32_t Conway::next_heatmap_comp_time_ = 0;

uint16_t Conway::update_delay = 1000;

void Conway::resetMap(void) {
  board = board1;
  otherBoard = board2;
  // this method can be used as a way to work around an existing bug with a single key
  // getting special attention or if the user just wants a button to reset the map
  for (uint8_t r = 0; r < ROWS * 3; r++) {
    for (uint8_t c = 0; c < COLS; c++) {
      board1[r][c] = 0;
      board2[r][c] = 0;
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


  board[row * 3][col] = 1;
  board[row * 3 + 1][col] = 1;
  board[row * 3 + 2][col] = 1;

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
      uint8_t red = board[r * 3][c];
      uint8_t green = board[r * 3 + 1][c];
      uint8_t blue = board[r * 3 + 2][c];

      // set the LED color accordingly
      ::LEDControl.setCrgbAt(r, c, computeColor(red, green, blue));
    }
  }

  stepBoard();
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
            aliveNeighbours += board[(l + i + ROWS * 3) % (ROWS * 3)][(m + j + COLS) % COLS];
          }
        }
      }

      // Implementing the Rules of Life

      // Cell is lonely and dies
      uint8_t cell = board[l][m];
      if ((aliveNeighbours < 2) || (aliveNeighbours > 3)) {
        otherBoard[l][m] = 0;
      }
      // A new cell is born
      else if ((cell == 0) && (aliveNeighbours == 3)) {
        otherBoard[l][m] = 1;
      }
      // Remains the same
      else {
        otherBoard[l][m] = cell;
      }
    }
  }

  uint8_t (*temp)[COLS] = board;
  board = otherBoard;
  otherBoard = temp;
}
}

kaleidoscope::Conway ConwayEffect;
