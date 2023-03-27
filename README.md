# ✨ AlphaLED

## ⚡ Usage
In Arduino sketch, set your leds numbers map, then in loop function use the **animate(startMap, endMap)** <br />

Available functions:
- **animate()**:
    - **required** startMap (int8_t): map for animation start
    - **required** endMap (int8_t): map for animation end
    - Direction (0/1) *default is 0 - left-to-right*,
    - Gaps between frames (in miliseconds) *default is 100*

- **display()**:
    - **required** map (int8_t): map to display