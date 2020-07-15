import socket
import json
import time

_s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


_timeout = 2  # second before next isAlive request
_start_time = time.time()


class Commands:  # describes all implemented commands
    class Alive:
        _cmd = "alive"
        _args = []

    class Arm:
        _cmd = "arm"
        _args = []

    class Disarm:
        _cmd = "disarm"
        _args = []

    class Exit:
        _cmd = "exit"
        _args = []


def send_cmd(cmd):
    packet = {
        "cmd": cmd._cmd
    }
    _s.send(bytes(json.dumps(packet), 'UTF8'))


def watchdog():
    global _start_time
    send_cmd(Commands.Alive)
    _start_time = time.time()


def spam():
    while True:
        if (time.time() - _start_time) > _timeout:
            watchdog()
        try:
            data = _s.recv(1024)
            if len(data) > 0:
                converted = json.loads(data)
                print(converted)
        except socket.timeout as msg:
            continue


def connect():
    _s.connect(("localhost", 27015))
    _s.settimeout(0.1)
    print("connected")


def disconnect():
    _s.close()


def read():
    while True:
        try:
            data = _s.recv(1024)
            if len(data) > 0:
                return json.loads(data)
        except socket.timeout:
            continue
    return None
