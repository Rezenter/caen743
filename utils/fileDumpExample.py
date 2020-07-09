filename = 'log.txt'
count = 100000

with open(filename, 'w') as log:
    for dummy in range(count):

        load = ['s' + str(d) for d in range(count)]  # some work for the CPU to slow down iteration

        log.write('%d\n' % dummy)  # write data to file IO buffer
        log.flush()  # force write IO buffer to disk

        print(dummy)
