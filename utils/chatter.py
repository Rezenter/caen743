import socket
import json

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# now connect to the web server on port 80 - the normal http port
s.connect(("localhost", 27015))


class Commands:  # describes all implemented commands
    class Arm:
        _cmd = "arm"
        _args = []

    class Alive:
        _cmd = "alive"
        _args = []


def sendCmd(cmd):
    packet = {
        "cmd": cmd._cmd
    }
    s.send(bytes(json.dumps(packet), 'UTF8'))


sendCmd(Commands.Alive)

data = s.recv(1024)

s.close()

print(repr(data))
