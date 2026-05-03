# Memory Card Save and Load with Animated Icon
## RAM Version
This sample demonstrates creating a save game file on the **Net Yaroze / PlayStation** memory card with a 3-frame animated save game icon.

---

## Screenshot

![Save Screenshot](https://raw.githubusercontent.com/logi-26/Net-Yaroze-Samples/main/samples/memory%20card/memory%20card%20ram/screenshot.png)



![Bios Screenshot](https://raw.githubusercontent.com/logi-26/Net-Yaroze-Samples/main/samples/memory%20card/memory%20card%20ram/screenshot1.png)
---

## How It Works

The project loads 3 16x16 TIM files through the `AUTO` file:

```txt
local dload data\icon1.tim 800C8000
local dload data\icon2.tim 800C9000
local dload data\icon3.tim 800CA000