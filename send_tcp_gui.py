#!/usr/bin/env python3
import tkinter as tk
import math
import socket
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
        self._update_indicator(angle_rad)

    def _update_indicator(self, angle_rad):
        angle_deg = (math.degrees(angle_rad) + 360) % 360
        self.angle = int(angle_deg)
        x = self.center[0] + self.radius * math.cos(angle_rad)
        y = self.center[1] - self.radius * math.sin(angle_rad)
        self.coords(self.indicator, self.center[0], self.center[1], x, y)
        if self.callback:
            self.callback(self.angle)

    def set_angle(self, angle_deg):
        rad = math.radians(angle_deg)
        self._update_indicator(rad)


class MotorControlApp:
    def __init__(self, host, port):
        # TCP‐клиент
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print(f"Connecting to {host}:{port} …")
        self.sock.connect((host, port))
        time.sleep(0.1)

        self.speed = 0.0
        self.direction = 0

        # GUI
        self.root = tk.Tk()
        self.root.title("ESP32 Motor Wi-Fi Control")

        # вертикальный ползунок скорости
        self.speed_slider = tk.Scale(
            self.root, from_=100, to=0, label="Speed (%)",
            orient=tk.VERTICAL, length=200,
            command=self.on_speed_change
        )
        self.speed_slider.set(self.speed)
        self.speed_slider.grid(row=0, column=0, rowspan=3, padx=20, pady=20)

        # круговой регулятор направления
        self.dial = Dial(self.root, radius=80, callback=self.on_direction_change)
        self.dial.grid(row=0, column=1, rowspan=3, padx=20, pady=20)

        # кнопки для быстрых направлений
        btn_frame = tk.Frame(self.root)
        btn_frame.grid(row=3, column=0, columnspan=2, pady=10)
        tk.Button(btn_frame, text="↑", width=5,
                  command=lambda: self.set_direction(0)).grid(row=0, column=1)
        tk.Button(btn_frame, text="←", width=5,
                  command=lambda: self.set_direction(270)).grid(row=1, column=0)
        tk.Button(btn_frame, text="→", width=5,
                  command=lambda: self.set_direction(90)).grid(row=1, column=2)
        tk.Button(btn_frame, text="↓", width=5,
                  command=lambda: self.set_direction(180)).grid(row=2, column=1)

    def on_speed_change(self, val):
        self.speed = float(val)
        self.send_command()

    def on_direction_change(self, angle):
        self.direction = angle
        self.send_command()

    def set_direction(self, angle):
        self.dial.set_angle(angle)

    def send_command(self):
        cmd = f"S:{self.speed:.1f},D:{self.direction}\n"
        print("Sending:", cmd.strip())
        try:
            self.sock.send(cmd.encode('ascii'))
        except Exception as e:
            print("Error sending:", e)

    def run(self):
        self.root.protocol("WM_DELETE_WINDOW", self.on_close)
        self.root.mainloop()

    def on_close(self):
        try:
            self.sock.close()
        except:
            pass
        self.root.destroy()


def main():
    parser = argparse.ArgumentParser(description="ESP32 Motor Wi-Fi GUI Control")
    parser.add_argument("--host", default="192.168.1.100",
                        help="ESP32 IP address")
    parser.add_argument("--port", type=int, default=5000,
                        help="ESP32 TCP port")
    args = parser.parse_args()

    app = MotorControlApp(args.host, args.port)
    app.run()


if __name__ == "__main__":
    main()
