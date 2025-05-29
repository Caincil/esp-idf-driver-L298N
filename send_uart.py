#!/usr/bin/env python3
import tkinter as tk
import math
import serial
import time
import argparse

class Dial(tk.Canvas):
    def __init__(self, master, radius=80, callback=None, **kwargs):
        super().__init__(master, width=2*radius+20, height=2*radius+20, **kwargs)
        self.radius = radius
        self.center = (radius+10, radius+10)
        self.callback = callback
        self.angle = 0
        # круг
        self.create_oval(10, 10, 10+2*radius, 10+2*radius, outline="black")
        # стрелка
        self.indicator = self.create_line(
            self.center[0], self.center[1],
            self.center[0] + radius, self.center[1],
            width=3, fill="red"
        )
        self.bind("<Button-1>", self._on_drag)
        self.bind("<B1-Motion>", self._on_drag)

    def _on_drag(self, event):
        dx = event.x - self.center[0]
        dy = self.center[1] - event.y  # инвертируем y
        angle_rad = math.atan2(dy, dx)
        angle_deg = (math.degrees(angle_rad) + 360) % 360
        self.angle = int(angle_deg)
        # обновляем стрелку
        x = self.center[0] + self.radius * math.cos(angle_rad)
        y = self.center[1] - self.radius * math.sin(angle_rad)
        self.coords(self.indicator, self.center[0], self.center[1], x, y)
        if self.callback:
            self.callback(self.angle)

class MotorControlApp:
    def __init__(self, port, baud):
        self.ser = serial.Serial(port, baud, timeout=1)
        time.sleep(2)  # даём ESP32 перезагрузиться
        self.speed = 0.0
        self.direction = 0

        self.root = tk.Tk()
        self.root.title("ESP32 Motor Control")

        # Вертикальный ползунок скорости
        self.speed_slider = tk.Scale(
            self.root, from_=100, to=0, label="Speed (%)",
            orient=tk.VERTICAL, length=200,
            command=self.on_speed_change
        )
        self.speed_slider.set(self.speed)
        self.speed_slider.pack(side=tk.LEFT, padx=20, pady=20)

        # Круговой регулятор направления
        self.dial = Dial(self.root, radius=80, callback=self.on_direction_change)
        self.dial.pack(side=tk.RIGHT, padx=20, pady=20)

    def on_speed_change(self, val):
        self.speed = float(val)
        self.send_command()

    def on_direction_change(self, angle):
        self.direction = angle
        self.send_command()

    def send_command(self):
        cmd = f"S:{self.speed:.1f},D:{self.direction}\n"
        print("Sending:", cmd.strip())
        self.ser.write(cmd.encode('ascii'))

    def run(self):
        self.root.mainloop()
        self.ser.close()

def main():
    parser = argparse.ArgumentParser(description="ESP32 Motor GUI Control")
    parser.add_argument("--port", default="/dev/ttyUSB0", help="Serial port")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate")
    args = parser.parse_args()
    app = MotorControlApp(args.port, args.baud)
    app.run()

if __name__ == "__main__":
    main()
