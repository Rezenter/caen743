import matplotlib.pyplot as plt
import json
import statistics

path = 'd:/data/fastDump/debug/'
group_count = 8
ch_count = 2

front_threshold = 0  # parrots

boards = [0]  # boards to be processed
channels = [0, 1]  # channels to be processed
invert = []  # channels to be inverted

shotn = 80
shot_folder = '%s%05d' % (path, shotn)


def find_rising(signal, is_trigger):
    if is_trigger:
        local_threshold = 0
    else:
        local_threshold = front_threshold
    for i in range(len(signal) - 1):
        if signal[i + 1] >= local_threshold > signal[i]:
            return i + (local_threshold - signal[i]) / (signal[i + 1] - signal[i])
        if signal[i + 1] <= -local_threshold < signal[i]:
            return i - (local_threshold + signal[i]) / (signal[i + 1] - signal[i])
    return -1


with open('%s/header.json' % shot_folder, 'r') as header:
    data = json.load(header)
    #board_count = len(data['boards'])
    freq = float(data['frequency'])  # GS/s
    time_step = 1 / freq  # nanoseconds
    event_len = data['eventLength']
    timeline_prototype = [0]
    while len(timeline_prototype) != event_len:
        timeline_prototype.append(timeline_prototype[-1] + time_step)

shifted = {}
for board_idx in boards:
    with open('%s/%d.json' % (shot_folder, board_idx), 'r') as board_file:
        data = json.load(board_file)
    print('board %d recorded %d events' % (board_idx, len(data)))
    ax1 = plt.subplot(len(boards), 1, boards.index(board_idx) + 1)
    shifted[board_idx] = []
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
                    front = find_rising(event['groups'][group_idx]['data'][ch_idx], True)
                    for i in range(len(local_timeline)):
                        local_timeline[i] -= front * time_step
                        shifted_event['timeline'].append(local_timeline[i])
                    shifted_event['fronts'][ch_num] = front * time_step
                else:
                    front = find_rising(event['groups'][group_idx]['data'][ch_idx], False)
                    shifted_event['fronts'][ch_num] = front * time_step
                for i in range(len(local_timeline)):
                    if ch_num in invert:
                        shifted_event['channels'][ch_num].append(-event['groups'][group_idx]['data'][ch_idx][i])
                    else:
                        shifted_event['channels'][ch_num].append(event['groups'][group_idx]['data'][ch_idx][i])
                plt.plot(local_timeline, shifted_event['channels'][ch_num])

        shifted[board_idx].append(shifted_event)
        plt.ylabel('voltage, parrot')
        plt.xlabel('timeline, ns')
        plt.title('ADC %d' % board_idx)


print('writing file...')
with open('%s/shifted.csv' % shot_folder, 'w') as shifted_file:
    line = ''
    for board_idx in boards:
        line += 't, '
        for ch in channels:
            line += 'b%dch%d, ' % (board_idx, ch)
    shifted_file.write(line[:-2] + '\n')
    for event in range(len(shifted[boards[0]])):
        for cell_idx in range(len(timeline_prototype)):
            line = ''
            for board_idx in boards:
                line += '%.4f, ' % shifted[board_idx][event]['timeline'][cell_idx]
                for ch in channels:
                    line += '%.2f, ' % shifted[board_idx][event]['channels'][ch][cell_idx]
            shifted_file.write(line[:-2] + '\n')


with open('%s/fronts.csv' % shot_folder, 'w') as fronts_file:
    line = ''
    for board_idx in boards:
        for ch in channels:
            line += 'b%dch%d, ' % (board_idx, ch)
    fronts_file.write(line[:-2] + '\n')
    for event in range(len(shifted[boards[0]])):
        line = ''
        for board_idx in boards:
            for ch in channels:
                line += '%.2f, ' % shifted[board_idx][event]['fronts'][ch]
        fronts_file.write(line[:-2] + '\n')

for board_idx in boards:
    print("board %d", board_idx)
    for ch in channels:
        if ch == 0:
            continue
        dt = [shifted[board_idx][event]['fronts'][ch] - shifted[board_idx][event]['fronts'][0]
              for event in range(len(shifted[boards[0]]))]
        print("ch %d, mean = %.3f, std = %.2f, ptp = %.2f" %
              (ch, sum(dt) / len(dt), statistics.stdev(dt), max(dt) - min(dt)))
print('OK')
plt.show()
