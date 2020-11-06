import matplotlib.pyplot as plt
import json
import statistics
import os
import ijson
import math

path = 'd:/data/fastDump/debug/'
HEADER_FILE = 'header'
FILE_EXT = 'json'
_group_count = 8
_ch_count = 2

board_idx = 0  # boards to be processed
channels = [1, 2]  # channels to be processed
prehistory_ns = 125  # ns

shot_filename = "shotn.txt"
with open(shot_filename, 'r') as shotn_file:
    line = shotn_file.readline()
    shotn = int(line)

print('Last shot #%d' % shotn)
shotn = 275  # manual override
print('Viewing shot #%d' % shotn)
shot_folder = '%s%05d' % (path, shotn)

channels.sort()
props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)


def read_file(board_id):
    if not os.path.isdir(shot_folder):
        print('Requested shotn is missing.')
        return {}
    if not os.path.isfile('%s/%d.%s' % (shot_folder, board_id, FILE_EXT)):
        print('Requested shot is missing requested board file.')
        return {}
    with open('%s/%d.%s' % (shot_folder, board_id, FILE_EXT), 'rb') as board_file:
        return [event['groups'] for event in ijson.items(board_file, 'item', use_float=True)]


def find_rising(signal, threshold):
    for i in range(len(signal) - 1):
        if signal[i + 1] >= threshold > signal[i]:
            return i + (threshold - signal[i]) / (signal[i + 1] - signal[i])
    return -1


def find_falling(signal, threshold):
    for i in range(len(signal) - 1):
        if signal[i] >= threshold > signal[i + 1]:
            return i + (signal[i] - threshold) / (signal[i] - signal[i + 1])
    return len(signal)


with open('%s/header.json' % shot_folder, 'r') as header:
    data = json.load(header)
    freq = float(data['frequency'])  # GS/s
    time_step = 1 / freq  # nanoseconds
    timeline_prototype = [time_step * cell_index for cell_index in range(data['eventLength'])]
    trigger_threshold = data['triggerThreshold']
    prehistory_length = int(prehistory_ns // time_step) + 1



    print('Reading file...')
    data = read_file(board_idx)
    print('board %d recorded %d events' % (board_idx, len(data)))
    fig, ax = plt.subplots()
    processed_data = []

    for event in data:
        front = find_rising(event[0]['data'][0], trigger_threshold)
        shifted_event = {
            'timeline': [timeline_prototype[i] - front * time_step for i in range(len(timeline_prototype))],
            'channels': {},
            'zero': [],
            'std': [],
            'max': [],
            'fwhm': [],
            'delay': []
        }
        for group_idx in range(_group_count):
            for ch_idx in range(_ch_count):
                ch_num = ch_idx + group_idx * 2
                if ch_num not in channels:
                    continue
                zero = statistics.fmean(event[group_idx]['data'][ch_idx][:prehistory_length])
                shifted_event['channels'][ch_num] = []
                maximum = 0
                for i in range(len(shifted_event['timeline'])):
                    shifted_event['channels'][ch_num].append(event[group_idx]['data'][ch_idx][i] - zero)
                    maximum = max(maximum, shifted_event['channels'][ch_num][-1])
                plt.plot(shifted_event['timeline'], shifted_event['channels'][ch_num], alpha=0.1)
                shifted_event['zero'].append(zero)
                shifted_event['std'].\
                    append(statistics.stdev(event[group_idx]['data'][ch_idx][:prehistory_length], zero))
                shifted_event['max'].append(maximum)
                shifted_event['delay'].append(find_rising(event[group_idx]['data'][ch_idx], maximum / 2.0))
                shifted_event['fwhm'].\
                    append(find_falling(event[group_idx]['data'][ch_idx][math.floor(shifted_event['delay'][-1]):],
                                        maximum / 2.0))
        processed_data.append(shifted_event)



    plt.ylabel('signal, mV')
    plt.xlabel('timeline, ns')
    plt.title('ADC %d (starting from 0)' % board_idx)
    plt.xlim(-50, 100)
    plt.ylim(-100, 1000)
    filename = '../figures/board%d_ch%s' % (board_idx, channels)
    plt.savefig('%s.png' % filename)

    with open('%s.csv' % filename, 'w') as out_file:
        out_file.write('ch, zero, std, prehistory_std, std, max, std, fwhm, std, delay, std\n')
        for ch_ind in range(len(channels)):
            line = '%02d, ' % channels[ch_idx]
            tmp = [event['zero'][ch_idx] for event in processed_data]
            line += '%.5f, ' % statistics.fmean(tmp)

            tmp = [event['max'][ch_idx] for event in processed_data]
            print(ch_idx, tmp[0])

            line += '%.5f, ' % statistics.fmean(tmp)
            out_file.write('%s\n' % line)
    print('OK')
    plt.show()



