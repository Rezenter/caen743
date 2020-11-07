from utils import chatter
import time

shot_filename = "shotn.txt"
isPlasma = False
with open(shot_filename, 'r') as shotn_file:
    line = shotn_file.readline()
    shotn = int(line)


def increment_shotn():
    global shotn
    with open(shot_filename, 'w') as shotn_file:
        shotn_file.seek(0)
        shotn += 1
        shotn_file.write('%d' % shotn)


print(shotn)

chatter.connect()

chatter.send_cmd(chatter.Commands.Disarm)
print(chatter.read())

#chatter.send_cmd(chatter.Commands.Exit)

#time.sleep(2)

chatter.send_cmd(chatter.Commands.Close)
time.sleep(0.5)
chatter.disconnect()

print('Normal acquisition exit')

from utils import viewer