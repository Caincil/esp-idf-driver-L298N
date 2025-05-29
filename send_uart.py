#!/usr/bin/env python3
import serial
import time
import argparse

def main(port, baud, speed, direction, delay):
    # Открываем последовательный порт
    ser = serial.Serial(port, baud, timeout=1)
    time.sleep(2)  # даём ESP32 время на перезагрузку

    # Формируем и отправляем команду
    cmd = f"S:{speed:.1f},D:{direction}\n"
    print(f"Sending: {cmd.strip()}")
    ser.write(cmd.encode('ascii'))

    # При необходимости ждём перед закрытием
    time.sleep(delay)
    ser.close()

if __name__ == "__main__":
    p = argparse.ArgumentParser(description="Send speed/dir to ESP32 over UART")
    p.add_argument("--port",   default="/dev/ttyUSB0", help="UART device (e.g. COM3 или /dev/ttyUSB0)")
    p.add_argument("--baud",   type=int, default=115200, help="baud rate")
    p.add_argument("--speed",  type=float, default=50.0, help="скорость 0–100")
    p.add_argument("--dir",    type=int,   default=0,    help="направление 0–359°")
    p.add_argument("--delay",  type=float, default=0.5,  help="время удержания порта открытым")
    args = p.parse_args()

    main(args.port, args.baud, args.speed, args.dir, args.delay)
