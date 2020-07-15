from utils import chatter
import time

chatter.connect()

chatter.send_cmd(chatter.Commands.Alive)

print(chatter.read())
print(chatter.read())

time.sleep(1)

shotn = 2
isPlasma = False

chatter.send_cmd(chatter.Commands.Arm, [shotn, isPlasma])
print(chatter.read())
print(chatter.read())

time.sleep(10)

chatter.send_cmd(chatter.Commands.Disarm)
print(chatter.read())
print(chatter.read())

#chatter.send_cmd(chatter.Commands.Exit)

#time.sleep(2)

chatter.send_cmd(chatter.Commands.Close)
time.sleep(0.5)
chatter.disconnect()

print('Normal python exit')
