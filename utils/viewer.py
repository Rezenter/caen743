import matplotlib.pyplot as plt
from os import listdir
import json

path = 'd:/data/fastDump/debug/'
group_count = 8
ch_count = 2

front_threshold = 1000  # parrots

#channels = [0, 3, 4, 7, 8, 11, 12, 15]  # range(16)
#channels = [0, 1, 2, 3, 12, 13, 14, 15]  # range(16)
channels = [0, 15]  # range(16)

shotn = 15
shot_folder = '%s%05d' % (path, shotn)


def find_rising(signal, threshold):
    for i in range(len(signal) - 1):
        if (signal[i + 1] >= threshold > signal[i]) or (signal[i + 1] <= threshold < signal[i]):
            return i + (threshold - signal[i]) / (signal[i + 1] - signal[i])
    return -1


with open('%s/header.json' % shot_folder, 'r') as header:
    data = json.load(header)
    board_count = len(data['boards'])
    freq = data['frequency']  # GS/s
    time_step = 1 / freq  # nanoseconds
    event_len = data['eventLength']
    timeline_prototype = [0]
    while len(timeline_prototype) != event_len:
        timeline_prototype.append(timeline_prototype[-1] + time_step)

shifted = []
for board_idx in range(board_count):
    with open('%s/%d.json' % (shot_folder, board_idx), 'r') as board_file:
        data = json.load(board_file)
    print('board %d recorded %d events' % (board_idx, len(data)))
    ax1 = plt.subplot(board_count, 1, board_idx + 1)
    shifted.append([])
    for event in data:
        shifted_event = {
            'timeline': [],
            'channels': {},
            'fronts': {}
        }
        local_timeline = timeline_prototype.copy()
        for group_idx in range(group_count):
            for ch_idx in range(ch_count):
                ch_num = ch_idx + group_idx * 2
                if ch_num not in channels:
                    continue
                shifted_event['channels'][ch_num] = []
                if ch_num == 0:
                    front = find_rising(event['groups'][group_idx]['data'][ch_idx], front_threshold)
                    for i in range(len(local_timeline)):
                        local_timeline[i] -= front * time_step
                        shifted_event['timeline'].append(local_timeline[i])
                    shifted_event['fronts'][ch_num] = front * time_step
                else:
                    front = find_rising(event['groups'][group_idx]['data'][ch_idx], front_threshold)
                    shifted_event['fronts'][ch_num] = front * time_step
                for i in range(len(local_timeline)):
                    shifted_event['channels'][ch_num].append(event['groups'][group_idx]['data'][ch_idx][i])
                plt.plot(local_timeline, event['groups'][group_idx]['data'][ch_idx])

        shifted[board_idx].append(shifted_event)
        plt.ylabel('voltage, parrot')
        plt.xlabel('timeline, ns')
        plt.title('ADC %d' % board_idx)


print('writing file...')
with open('%s/shifted.csv' % shot_folder, 'w') as shifted_file:
    line = ''
    for board_idx in range(board_count):
        line += 't, '
        for ch in channels:
            line += 'b%dch%d, ' % (board_idx, ch)
    shifted_file.write(line[:-2] + '\n')
    for event in range(len(shifted[0])):
        for cell_idx in range(len(timeline_prototype)):
            line = ''
            for board_idx in range(board_count):
                line += '%.1f, ' % shifted[board_idx][event]['timeline'][cell_idx]
                for ch in channels:
                    line += '%.2f, ' % shifted[board_idx][event]['channels'][ch][cell_idx]
            shifted_file.write(line[:-2] + '\n')


with open('%s/fronts.csv' % shot_folder, 'w') as fronts_file:
    line = ''
    for board_idx in range(board_count):
        for ch in channels:
            line += 'b%dch%d, ' % (board_idx, ch)
    fronts_file.write(line[:-2] + '\n')
    for event in range(len(shifted[0])):
        line = ''
        for board_idx in range(board_count):
            for ch in channels:
                line += '%.2f, ' % shifted[board_idx][event]['fronts'][ch]
        fronts_file.write(line[:-2] + '\n')

print('OK')
plt.show()
