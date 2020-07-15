from utils import chatter
import time

chatter.connect()

chatter.send_cmd(chatter.Commands.Alive)

print(chatter.read())
print(chatter.read())

time.sleep(2)

#chatter.send_cmd(chatter.Commands.Exit)

#time.sleep(2)

chatter.disconnect()

