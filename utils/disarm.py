from utils import chatter
import time


chatter.connect()

chatter.send_cmd(chatter.Commands.Disarm)
print(chatter.read())

chatter.send_cmd(chatter.Commands.Close)
time.sleep(0.5)
chatter.disconnect()

print('Normal acquisition exit')
