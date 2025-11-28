import serial
import time
import numpy as np
import json

PORT = "COM17"
BAUD = 115200

print(f"Opening {PORT}...")
ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)
print("Serial connected.\n")

adc_points = []
real_points = []

print("=== ADC Calibration ===")
print("Type the REAL voltage (e.g., 4.20) or 'end' to finish.\n")

while True:
    entry = input("Real value (or 'end'): ").strip()

    if entry.lower() == "end":
        break

    try:
        real_value = float(entry)
    except ValueError:
        print("Invalid value.\n")
        continue

    print(f"\n→ Collecting samples for {real_value} V...")

    ser.reset_input_buffer()

    samples = []
    N = 200  # number of samples per calibration point

    while len(samples) < N:
        line = ser.readline().decode(errors="ignore").strip()

        if "," not in line:
            continue

        try:
            raw_str, mv_str = line.split(",")
            raw = int(raw_str)
        except:
            continue

        samples.append(raw)

    avg_raw = np.mean(samples)
    print(f"Average ADC = {avg_raw:.2f}")

    adc_points.append(avg_raw)
    real_points.append(real_value)

print("\n=== Calculating Fit Curve ===")
coef = np.polyfit(adc_points, real_points, 1)
a, b = coef

print(f"\nEquation:  V = {a:.10f} * RAW + {b:.10f}")
