import matplotlib.pyplot as plt
import json
import statistics
import os
import ijson
import math


path = 'd:/data/fastDump/plasma/'
HEADER_FILE = 'header'
FILE_EXT = 'json'
_group_count = 8
_ch_count = 2

shot_filename = "shotn.txt"
with open(shot_filename, 'r') as shotn_file:
    line = shotn_file.readline()
    shotn = int(line)

print('Last shot #%d' % shotn)
shotn = 294  # manual override
print('Viewing shot #%d' % shotn)
shot_folder = '%s%05d' % (path, shotn)
prehistory_ns = 125  # ns
integrate_start_ns = 5 # ns

poly_map = [
    {
        'fiber': '1A',
        'board': 0,
        'channels': [1, 2, 3, 4, 5],
        'raman': 6
    },
    {
        'fiber': '1B',
        'board': 0,
        'channels': [6, 7, 8, 9, 10],
        'raman': 7
    },
    {
        'fiber': '2',
        'board': 0,
        'channels': [11, 12, 13, 14, 15],
        'raman': 8
    },
    {
        'fiber': '3',
        'board': 1,
        'channels': [1, 2, 3, 4, 5],
        'raman': 9
    },
    {
        'fiber': '4',
        'board': 1,
        'channels': [6, 7, 8, 9, 10],
        'raman': 10
    },
    {
        'fiber': '5',
        'board': 1,
        'channels': [11, 12, 13, 14, 15],
        'raman': 11
    },
    {
        'fiber': '6',
        'board': 2,
        'channels': [1, 2, 3, 4, 5],
        'raman': 12
    },
    {
        'fiber': '7',
        'board': 2,
        'channels': [6, 7, 8, 9, 10],
        'raman': 13
    },
    {
        'fiber': '8',
        'board': 2,
        'channels': [11, 12, 13, 14, 15],
        'raman': 14
    },
    {
        'fiber': '9',
        'board': 3,
        'channels': [1, 2, 3, 4, 5],
        'raman': 15
    }
]


def process(poly, board_idx, channels):
    channels.sort()

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

    def write_stats(filename, processed_data):
        print('Writing stats file...')
        with open(filename, 'w') as out_file:
            out_file.write('ch, zero (mV), std (mV), prehistory_std (mV), std (mV), max (mV), std (mV), '
                           'integral (mV*ns), std (mV*ns), delay (ns), std (ns), fwhm (ns), std (ns)\n')
            for ch_ind in range(len(channels)):
                line = '%02d, ' % channels[ch_ind]
                # zero
                tmp = [event['zero'][ch_ind] for event in processed_data]
                value_m = statistics.fmean(tmp)
                line += '%.2f, ' % value_m
                line += '%.2f, ' % statistics.stdev(tmp, value_m)

                # prehistory_std
                tmp = [event['std'][ch_ind] for event in processed_data]
                value_m = statistics.fmean(tmp)
                line += '%.3f, ' % value_m
                line += '%.3f, ' % statistics.stdev(tmp, value_m)

                # max
                tmp = [event['max'][ch_ind] for event in processed_data]
                value_m = statistics.fmean(tmp)
                line += '%.1f, ' % value_m
                line += '%.1f, ' % statistics.stdev(tmp, value_m)

                # integral
                tmp = [event['integral'][ch_ind] for event in processed_data]
                value_m = statistics.fmean(tmp)
                line += '%.1f, ' % value_m
                line += '%.1f, ' % statistics.stdev(tmp, value_m)

                # delay
                tmp = [event['delay'][ch_ind] for event in processed_data]
                value_m = statistics.fmean(tmp)
                line += '%.2f, ' % value_m
                line += '%.2f, ' % statistics.stdev(tmp, value_m)

                # fwhm
                tmp = [event['fwhm'][ch_ind] for event in processed_data]
                value_m = statistics.fmean(tmp)
                line += '%.2f, ' % value_m
                line += '%.2f' % statistics.stdev(tmp, value_m)

                out_file.write('%s\n' % line)
        print('Done.')

    def average(processed_data):
        print('Averaging...')

        shortest = 1023
        longest = 0
        for event in processed_data:
            shortest = min(shortest, event['trigger_cell'])
            longest = max(longest, event['trigger_cell'])
        left = math.floor(shortest)
        right = len(timeline_prototype) - math.floor(longest)
        averaged = [[
            0 for cell in range(left + right)
        ] for ch in channels]

        for event in processed_data:
            start_ind = math.floor(event['trigger_cell']) - left
            for ch_ind in range(len(channels)):
                for cell in range(len(averaged[ch_ind])):
                    averaged[ch_ind][cell] += event['channels'][channels[ch_ind]][cell + start_ind]

        fig, ax = plt.subplots()
        timeline = [(cell - left) * time_step for cell in range(len(averaged[0]))]
        for ch_ind in range(len(channels)):
            for cell in range(len(averaged[ch_ind])):
                averaged[ch_ind][cell] /= len(processed_data)
            plt.plot(timeline, averaged[ch_ind], label='ADC_ch %d' % channels[ch_ind])

        plt.ylabel('signal, mV')
        plt.xlabel('timeline, ns')
        plt.title('Averaged ADC %d (starting from 0)' % board_idx)
        plt.xlim(-5, 175)
        plt.ylim(-50, 2300)
        filename = '../figures/poly%d/ave_board%d_ch%s' % (poly, board_idx, channels)
        ax.legend()
        plt.grid(color='k', linestyle='-', linewidth=1)
        plt.savefig('%s.png' % filename, dpi=600)
        plt.close(fig)
        print('Done.')

    def integrate(start, array):  # ns*mV
        res = 0.0
        for i in range(start, len(array) - 1):
            res += time_step * (array[i] + array[i + 1]) * 0.5  # ns*mV
            if i > start + integrate_overshoot and array[i + 1] < 0:
                break
        else:
            print('Warning! Integration failed to stop.')
        #print((i - start) * time_step)
        return res

    with open('%s/header.json' % shot_folder, 'r') as header:
        data = json.load(header)
        freq = float(data['frequency'])  # GS/s
        time_step = 1 / freq  # nanoseconds
        timeline_prototype = [time_step * cell_index for cell_index in range(data['eventLength'])]
        trigger_threshold = data['triggerThreshold']
        prehistory_length = int(prehistory_ns // time_step) + 1
        integrate_overshoot = int(integrate_start_ns // time_step) + 1

        print('Reading file...')
        data = read_file(board_idx)
        print('board %d recorded %d events' % (board_idx, len(data)))
        fig, ax = plt.subplots()
        processed_data = []

        for event in data:
            front_ind = find_rising(event[0]['data'][0], trigger_threshold)
            front = front_ind * time_step
            shifted_event = {
                'timeline': [timeline_prototype[i] - front for i in range(len(timeline_prototype))],
                'trigger_cell': front_ind,
                'sync': front,
                'channels': {},
                'zero': [],
                'std': [],
                'max': [],
                'integral': [],
                'delay': [],
                'fwhm': []
            }
            for group_idx in range(_group_count):
                for ch_ind in range(_ch_count):
                    ch_num = ch_ind + group_idx * 2
                    if ch_num not in channels:
                        continue
                    zero = statistics.fmean(event[group_idx]['data'][ch_ind][:prehistory_length])
                    shifted_event['channels'][ch_num] = []
                    maximum = 0
                    for i in range(len(shifted_event['timeline'])):
                        shifted_event['channels'][ch_num].append(event[group_idx]['data'][ch_ind][i] - zero)
                        maximum = max(maximum, shifted_event['channels'][ch_num][-1])

                    plt.plot(shifted_event['timeline'], shifted_event['channels'][ch_num], alpha=0.1)
                    shifted_event['zero'].append(zero)
                    shifted_event['std']. \
                        append(statistics.stdev(event[group_idx]['data'][ch_ind][:prehistory_length], zero))
                    shifted_event['max'].append(maximum)
                    shifted_event['integral'].append(integrate(math.floor(front_ind) - integrate_overshoot,
                                                               shifted_event['channels'][ch_num]))
                    delay = find_rising(shifted_event['channels'][ch_num], maximum / 2.0)
                    shifted_event['delay'].append((delay - front_ind) * time_step)
                    shifted_event['fwhm']. \
                        append((find_falling(shifted_event['channels'][ch_num][math.floor(delay):],
                                             maximum / 2.0)) * time_step)

                    #print(shifted_event['timeline'][math.floor(front_ind) - integrate_overshoot])
                    print('%.2f' % shifted_event['integral'][0])
                    #print('%.2f' % shifted_event['max'][0])
            processed_data.append(shifted_event)

        plt.ylabel('signal, mV')
        plt.xlabel('timeline, ns')
        plt.title('Raw ADC %d (starting from 0)' % board_idx)
        plt.xlim(-5, 175)
        plt.ylim(-50, 2300)
        os.mkdir('../figures/poly%d' % poly)
        filename = '../figures/poly%d/board%d_ch%s' % (poly, board_idx, channels)
        plt.grid(color='k', linestyle='-', linewidth=1)
        plt.savefig('%s.png' % filename, dpi=600)
        plt.close(fig)
        write_stats('%s.csv' % filename, processed_data)
        average(processed_data)
        print('OK')
        #plt.show()


process(0, 0, [0])

#process(11, 3, [ch for ch in range(6, 16)])

'''
counter = 0
for poly in poly_map:
    print('\n\nProcessing poly %d of %d...' % (counter, len(poly_map)))
    process(counter, poly['board'], poly['channels'])
    counter += 1
    plt.clf()
    gc.collect()
'''
