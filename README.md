# ✨ AlphaLED

## ⚡ Usage
In Arduino sketch, set your leds numbers map, then in loop function use the **animate(startMap, endMap)** <br />

Available functions:
- **animate()**:
    - **required** startMap (int8_t): map for animation start
    - **required** endMap (int8_t): map for animation end
    - Direction (0/1/2/3) *default is 0 - left-to-right, 2 is to up*,
    - New text color (strip.Color),
    - New background color (strip.Color)

- **display()**:
    - **required** map (int8_t): map to display