#!/usr/bin/env python3
import argparse
import json
import serial
import sys
import time

DEFAULT_PORT = "/dev/ttyUSB0"
DEFAULT_BAUD = 115200
ACK_TIMEOUT = 1.0

EXPRESSION_CHOICES = ["Happy", "Angry", "Sad", "Doubt", "Sleepy", "Neutral"]


def parse_args():
    parser = argparse.ArgumentParser(description="Send expression/speech commands to StackChan Core2 via serial JSON")
    parser.add_argument("--port", default=DEFAULT_PORT, help="Serial port (default: %(default)s)")
    parser.add_argument("--baud", type=int, default=DEFAULT_BAUD, help="Baud rate (default: %(default)d)")
    parser.add_argument("--expression", choices=EXPRESSION_CHOICES, help="Avatar expression to display")
    parser.add_argument("--speech", help="Text to show in the speech bubble")
    parser.add_argument("--face", type=int, choices=range(0, 3), metavar="{0,1,2}", help="Face index (0=Ataru,1=Ram,2=StackChan)")
    parser.add_argument("--palette", type=int, choices=range(0, 3), metavar="{0,1,2}", help="Palette index (0-2) to apply alongside the face")
    parser.add_argument("--duration", type=int, help="Speech bubble duration in milliseconds (0 disables automatic clear)")
    parser.add_argument("--clear", action="store_true", help="Immediately clear the speech bubble")
    parser.add_argument("--file", type=argparse.FileType("r"), help="Send JSON commands from a newline-delimited file")
    parser.add_argument("--delay", type=float, default=0.2, help="Delay between commands when using --file (seconds)")
    return parser.parse_args()


def build_command(args):
    cmd = {}
    if args.expression:
        cmd["expression"] = args.expression
    if args.speech is not None:
        cmd["speech"] = args.speech
    if args.face is not None:
        cmd["face"] = args.face
    if args.palette is not None:
        cmd["palette"] = args.palette
    if args.duration is not None:
        cmd["duration"] = args.duration
    if args.clear:
        cmd["clear"] = True
    return cmd


def send_command(ser, command):
    payload = json.dumps(command)
    ser.write(payload.encode("utf-8") + b"\n")
    ser.flush()
    deadline = time.time() + ACK_TIMEOUT
    while time.time() < deadline:
        line = ser.readline().decode("utf-8").strip()
        if not line:
            continue
        if line.startswith("OK"):
            return True, line
        if line.startswith("ERR"):
            return False, line
    return False, "timeout waiting for ACK"


def main():
    args = parse_args()
    if not args.file and not any([args.expression, args.speech is not None, args.face is not None, args.palette is not None, args.duration is not None, args.clear]):
        print("No command fields provided. Use --help for options.")
        sys.exit(1)

    try:
        with serial.Serial(args.port, args.baud, timeout=0.1, rtscts=False, dsrdtr=False) as ser:
            if args.file:
                time.sleep(1)
                for line in args.file:
                    line = line.strip()
                    if not line:
                        continue
                    try:
                        command = json.loads(line)
                    except json.JSONDecodeError as err:
                        print(f"Skipping invalid JSON line: {err}")
                        continue
                    ok, resp = send_command(ser, command)
                    print(f"> {command} -> {resp}")
                    if not ok:
                        sys.exit(1)
                    time.sleep(args.delay)
            else:
                command = build_command(args)
                time.sleep(1)
                ok, resp = send_command(ser, command)
                print(f"> {command} -> {resp}")
                if not ok:
                    sys.exit(1)
    except serial.SerialException as exc:
        print(f"Serial error: {exc}")
        sys.exit(1)


if __name__ == "__main__":
    main()
