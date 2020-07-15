from utils import chatter
import time

chatter.connect()

#chatter.send_cmd(chatter.Commands.Alive)
chatter.watchdog()

time.sleep(5)

chatter.read()

