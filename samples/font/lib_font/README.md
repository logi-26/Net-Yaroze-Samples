# Net Yaroze Pixel Font Rendering Module

A pixel font rendering module for the PlayStation Net Yaroze. Renders 8x8 bitmap font characters directly from RAM using `GsSortBoxFill`, with no VRAM texture uploads required. Supports multiple font styles, scaling, and a range of animated text effects.

---

## How It Works

The font spritesheet is a 4bpp indexed TIM file loaded into RAM via the AUTO file `dload` command. Characters are drawn pixel-by-pixel using `GsSortBoxFill`, giving per-pixel control that makes effects like outlines possible without any texture manipulation.

This is the font sheet provided on the Net Yaroze boot disc. It contains multiple font styles, special character and Japanese characters.

This code can be easily modified to use any other font sheet, but using a sheet with 8×8 pixel characters arranged in 32 columns means that only the font offsets would need adjusting for the number of rows in the sheet.

```
AUTO file setup:
    local dload data\8dotfont.tim 800C8000
```

The font sheet is 256px wide × 128px tall, with 8×8 pixel characters arranged in 32 columns, starting at ASCII 33.

---

## Setup

### 1. Add to your project

Copy `font.h` and `font.c` into your project directory and include the header:

```c
#include "font.h"
```

### 2. Initialise

Call `FontFX_LoadFont()` once before the main loop:

```c
FontFX_LoadFont();
```

### 3. Update each frame

Call `FontFX_Update()` at the start of your main loop to advance all effect timers:

```c
FontFX_Update();
```

### 4. Draw text

```c
FontFX_SetColour(255, 255, 255);
FontFX_SetStyle(FONT_STYLE_0);
FontFX_Print(16, 32, "HELLO WORLD", &WorldOT[buf], 0);
```

---

## Font Styles

| Constant | Offset | Description |
|---|---|---|
| `FONT_STYLE_0` | 0 | Uppercase |
| `FONT_STYLE_1` | 64 | Uppercase |
| `FONT_STYLE_2` | 128 | Uppercase |
| `FONT_STYLE_3` | 160 | Lowercase (no numbers) |
| `FONT_STYLE_4` | 192 | Lowercase (no numbers) |
| `FONT_STYLE_5` | 416 | Uppercase |
| `FONT_STYLE_6` | 448 | Lowercase (no numbers) |

---

## Core Functions

```c
void FontFX_LoadFont(void);
void FontFX_FontBegin(void);
void FontFX_FontEnd(void);
void FontFX_SetSize(int size);
void FontFX_SetColour(unsigned char r, unsigned char g, unsigned char b);
void FontFX_SetStyle(int offset);
void FontFX_DrawChar(int screenX, int screenY, int c, GsOT *ot, int priority);
void FontFX_Print(int x, int y, char *text, GsOT *ot, int pri);
void FontFX_PrintWithoutEffects(int screenX, int screenY, char *text, GsOT *ot, int priority);
void FontFX_Update(void);
```

### FontFX_SetSize

Sets the character scale. Each step doubles the pixel size.

| Value | Size |
|---|---|
| 1 | 8×8 (default) |
| 2 | 16×16 |
| 3 | 24×24 |

---

## Effects

Effects are applied using `FontFX_FontBegin()` / `FontFX_FontEnd()` blocks with `FontFX_Print()`, or via convenience wrapper functions.

### Using FontFX_Print directly to stack multiple effects

```c
FontFX_FontBegin();
FontFX_SetColour(255, 0, 0);
FontFX_SetStyle(FONT_STYLE_0);
FontFX_SetRainbow(4);
FontFX_SetWave(2, 2, 1);
FontFX_Print(16, 32, "STACKED EFFECTS", &WorldOT[buf], 0);
FontFX_FontEnd();
```

Multiple effects can be stacked in a single `FontFX_FontBegin()` / `FontFX_FontEnd()` block.

---

## Wrapper Functions
#### These  wrap the font effects into single function calls. These are ideal if you don't need to stack multiple effects.

### Rainbow

Cycles through the full colour spectrum per character.

```c
void FontFX_PrintRainbow(int x, int y, char *text, int style, int speed, GsOT *ot, int pri);
```

| Parameter | Description |
|---|---|
| `speed` | How fast the colours cycle |

---

### Flash

Rapidly pulses brightness between a minimum and maximum value.

```c
void FontFX_PrintFlash(int x, int y, char *text, int style, int r, int g, int b, int min, int max, int speed, GsOT *ot, int pri);
```

| Parameter | Description |
|---|---|
| `r` | Red colour value |
| `g` | Green colour value |
| `b` | Blue colour value |
| `min` | Minimum colour opacity |
| `max` | Maximum colour opacity |
| `speed` | How fast the colours flash |

---

### Pulse

Smoothly pulses brightness between a minimum and maximum value.

```c
void FontFX_PrintPulse(int x, int y, char *text, int style, int r, int g, int b, int min, int max, int speed, GsOT *ot, int pri);
```

| Parameter | Description |
|---|---|
| `r` | Red colour value |
| `g` | Green colour value |
| `b` | Blue colour value |
| `min` | Minimum colour opacity |
| `max` | Maximum colour opacity |
| `speed` | How fast the colours pulse |

---

### Wave

Animates characters up and down in a sine-like wave pattern.

```c
void FontFX_PrintWave(int x, int y, char *text, int style, int r, int g, int b, int amplitude, int speed, int spacing, GsOT *ot, int pri);
```

| Parameter | Description |
|---|---|
| `r` | Red colour value |
| `g` | Green colour value |
| `b` | Blue colour value |
| `amplitude` | Height of the wave in pixels |
| `speed` | How fast the wave moves |
| `spacing` | Phase offset between characters |

---

### Shake

Randomly offsets each character position for a jitter effect.

```c
void FontFX_PrintShake(int x, int y, char *text, int style, int r, int g, int b, int intensity, int speed, GsOT *ot, int pri);
```

| Parameter | Description |
|---|---|
| `r` | Red colour value |
| `g` | Green colour value |
| `b` | Blue colour value |
| `intensity` | How far the chars move |
| `speed` | How fast the chars shake |

---

### Typewriter

Reveals characters one at a time from left to right.

```c
void FontFX_PrintTypewriter(int x, int y, char *text, int style, int r, int g, int b, int speed, GsOT *ot, int pri);
```

| Parameter | Description |
|---|---|
| `r` | Red colour value |
| `g` | Green colour value |
| `b` | Blue colour value |
| `speed` | How fast the chars are revealed |

---

### Scroll

Scrolls text horizontally across a defined screen region from right to left.

```c
void FontFX_PrintScroll(int x, int y, char *text, int style, int r, int g, int b, int width, int speed, GsOT *ot, int pri);
```

| Parameter | Description |
|---|---|
| `r` | Red colour value |
| `g` | Green colour value |
| `b` | Blue colour value |
| `width` | Screen width of the scroll region in pixels |
| `speed` | Pixels per tick to scroll |


---

### Outline

Draws each character with a solid 1-pixel outline in a configurable colour.

```c
void FontFX_PrintScroll(int x, int y, char *text, int style, int r, int g, int b, int width, int speed, GsOT *ot, int pri);
```

| Parameter | Description |
|---|---|
| `r` | Red colour value |
| `g` | Green colour value |
| `b` | Blue colour value |
| `or` | Red colour value for the outline |
| `og` | Green colour value for the outline |
| `ob` | Blue colour value for the outline |

---

### Fade

Animates character brightness from a start value to an end value. Supports one-shot and looping modes.

```c
void FontFX_PrintFade(int x, int y, char *text, int style, int r, int g, int b, int start, int end, int speed, int loop, GsOT *ot, int pri);
```

| Parameter | Description |
|---|---|
| `r` | Red colour value |
| `g` | Green colour value |
| `b` | Blue colour value |
| `start` | Starting brightness (0–255) |
| `end` | Ending brightness (0–255) |
| `speed` | Rate of change per tick |
| `loop` | `0` = one-shot, `1` = ping-pong loop |

```c
/* Fade in once */
FontPrintFade(16, 160, "GAME OVER", FONT_STYLE_0, 255, 0, 0, 0, 255, 1, 0, &WorldOT[buf], 0);

/* Fade out once */
FontPrintFade(16, 176, "PRESS START", FONT_STYLE_0, 255, 255, 255, 255, 0, 1, 0, &WorldOT[buf], 0);

/* Continuous fade-in and fade-out (similar to pulse) */
FontPrintFade(16, 192, "INSERT COIN", FONT_STYLE_0, 255, 255, 0, 0, 255, 2, 1, &WorldOT[buf], 0);
```

---

## Effect Flags

| Flag | Value | Effect |
|---|---|---|
| `FONT_FX_FLASH` | `1 << 0` | Flash |
| `FONT_FX_RAINBOW` | `1 << 1` | Rainbow |
| `FONT_FX_TYPE` | `1 << 2` | Typewriter |
| `FONT_FX_WAVE` | `1 << 3` | Wave |
| `FONT_FX_SHAKE` | `1 << 4` | Shake |
| `FONT_FX_PULSE` | `1 << 6` | Pulse |
| `FONT_FX_SCROLL` | `1 << 7` | Scroll |
| `FONT_FX_OUTLINE` | `1 << 8` | Outline |
| `FONT_FX_FADE` | `1 << 9` | Fade |

---

## Stacking Effects

Any combination of effects can be stacked inside a `FontFX_FontBegin()` / `FontFX_FontEnd()` block:

```c
FontFX_FontBegin();
FontFX_SetColour(255, 0, 0);
FontFX_SetStyle(FONT_STYLE_1);
FontFX_SetFlash(0, 255, 80);
FontFX_SetRainbow(5);
FontFX_SetWave(2, 2, 1);
FontFX_Print(16, 226, "MULTI EFFECT", &WorldOT[buf], 0);
FontFX_FontEnd();
```

---

## Technical Notes

- Characters are rendered pixel-by-pixel using `GsSortBoxFill`. No VRAM upload is required.
- The font TIM is read directly from RAM at `0x800C8000`.
- Font data is 4bpp: each byte stores two pixels. Value `0` = draw pixel, non-zero = transparent.
- All effect timers are global and advance every frame via `FontFX_Update()`.
- `FontFX_FontBegin()` resets the effect flags. `FontFX_FontEnd()` clears them after drawing.
- The ordering table must be long enough to support `priority + 1` when using the outline effect.


## Video Showcase

[![Watch on YouTube](https://img.youtube.com/vi/mIURQJthwrY/0.jpg)](https://www.youtube.com/watch?v=mIURQJthwrY)
