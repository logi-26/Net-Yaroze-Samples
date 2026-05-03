# Font Rendering

This sample demonstrates how to render custom bitmap text on the **Net Yaroze / PlayStation** using a sprite-sheet font loaded directly into RAM.

Instead of using the built-in `FntPrint()` debug font, this project reads character pixels from a **4bpp TIM image** and draws each pixel manually using `GsSortBoxFill()`. This allows complete control over:

- Custom font styles
- Per-text RGB colours
- Multiple font banks
- Lightweight rendering without VRAM uploads
- Reusable text drawing routines

---

## Screenshot

![Font Screenshot](https://raw.githubusercontent.com/logi-26/Net-Yaroze-Samples/main/samples/font/basic/screenshot.png)

---

## How It Works

The project loads a TIM file through the `AUTO` file:

```txt
local dload data\8dotfont.tim 800C8000