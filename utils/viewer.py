import matplotlib.pyplot as plt
from os import listdir

path = '../cmake-build-debug/'
header_length = 2
ommit = []#[1, 2]

count = 1

for address in range(4):
    if address in ommit:
        continue
    ax1 = plt.subplot(4 - len(ommit), 1, count)
    count += 1
    for f in listdir('%s%d' % (path, address)):
        event = int(f[:-4])
        data = [[] for ch in range(16)]
        with open('%s%d/%d.csv' % (path, address, event), 'r') as file:
            for skip in range(header_length):
                file.readline()
            for line in file:
                time_point = line.split(',')
                for ch in range(16):
                    data[ch].append(float(time_point[ch + 1]))

        plt.plot(data[0])
        break
    plt.ylabel('voltage, mV')
    plt.xlabel('cell Index, 1')
    plt.title('ADC %d' % address)

plt.show()
