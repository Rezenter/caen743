import matplotlib.pyplot as plt
from os import listdir
import json

path = 'd:/data/fastDump/debug/'
group_count = 8
ch_count = 2

channels = [0]  # range(16)

shotn = 6
shot_folder = '%s%05d' % (path, shotn)


#def


with open('%s/header.json' % shot_folder) as header:
    data = json.load(header)
    board_count = len(data['boards'])
    freq = data['frequency']  # GS/s
    time_step = 1 / freq  # nanoseconds
    event_len = data['eventLength']
    local_timeline = [0]
    while len(local_timeline) != event_len:
        local_timeline.append(local_timeline[-1] + time_step)

for board_idx in range(board_count):
    with open('%s/%d.json' % (shot_folder, board_idx)) as board_file:
        data = json.load(board_file)
        print('board %d recorded %d events' % (board_idx, len(data)))
        ax1 = plt.subplot(board_count, 1, board_idx + 1)
        for event in data:
            for group_idx in range(group_count):
                for ch_idx in range(ch_count):
                    ch_num = ch_idx + group_idx * 2
                    if ch_num not in channels:
                        continue
                    plt.plot(local_timeline, event['groups'][group_idx]['data'][ch_idx])

        plt.ylabel('voltage, parrot')
        plt.xlabel('timeline, ns')
        plt.title('ADC %d' % board_idx)

plt.show()
