# 🍹 Automated Beverage Kiosk (Embedded Systems)

A **microcontroller-driven juice mixer** that blends up to 4 juices in precise, customizable ratios with **Auto** and **Manual** modes—built end-to-end from **PCB** to **firmware** to **enclosure**.  
Hygienic peristaltic pumps + intuitive UI = **consistent pours every time**.

---

## 🧩 Features

- **Brains**: ATmega328P (AVR)
- **Actuation**: 4 × 12 V peristaltic pumps via 4-ch relay module
- **Power**: 220 V → 12 V supply + 12 V → 5 V buck for logic
- **UI / Inputs**: 16×2 I²C LCD + rotary encoder + push buttons
- **Plumbing**: 1 mm transparent tubing
- **Stack**: AVR-GCC + AVRDUDE; custom I²C LCD driver

---

## ⚙️ How It Works

### Auto Mode
- Pick percentages for each fruit in **0 / 20 / 40 / 60 / 80 / 100%** steps.  
- System validates the total ≤ 100%.  
- Dispenses automatically with calibrated timing.

### Manual Mode
- Select one fruit and dispense continuously.  
- Button press fires a **PCINT interrupt** to stop instantly → great for on-demand pours.

---

## 🧪 Calibration & Results

- Calibration performed with a **250 ml cup** marked at 50 ml steps (maps to 20% increments).  
- Measured average dispense times per step → **linear, repeatable pours**:  
  - 20% → 2.18 s  
  - 40% → ~4.00 s  
  - 60% → ~6.00 s  
  - 80% → ~7.10 s  
  - 100% → 8.11 s  
- **Result**: Consistent, repeatable volume control across all four pumps.

---

## 🛠️ Build Notes

- **Firmware**: Modular AVR C—encoder navigation, relay control, LCD screens (status + warnings), and interrupt-driven safety.  
- **Electronics**: Custom PCB for tidy wiring & reliability; dual-rail power (12 V + 5 V) inside a protected enclosure.  
- **Enclosure**: CNC-cut cladding boards, hot-glued assembly, clean front panel & dispensing area.  
- **Safety**:  
  - External interrupt (PCINT) for manual stop.  
  - Lessons learned → spacing & insulation to avoid shorts.

---

## 📸 Demo & Photos
> _(Add images or GIFs of your build here)_

---

## 🚀 Getting Started

### Hardware Setup
1. ATmega328P-based control board (custom PCB or Arduino Uno as dev board).
2. Connect peristaltic pumps via relay module.
3. Wire up 16×2 I²C LCD, rotary encoder, and buttons.
4. Power via 12 V adapter + 5 V buck converter for logic.

### Firmware Upload
```bash
# Compile with avr-gcc
avr-gcc -mmcu=atmega328p -Os -o main.elf main.c

# Convert to hex
avr-objcopy -O ihex -R .eeprom main.elf main.hex

# Upload with avrdude
avrdude -c arduino -P /dev/ttyUSB0 -p m328p -U flash:w:main.hex

#EmbeddedSystems #AVR #ATmega328P #IoT #Hardware #Firmware #Mechatronics
#ProductDesign #Automation #FoodTech #Prototyping #Calibrations #PCBDesign
