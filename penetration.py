#!/usr/bin/env python
# a bar plot with errorbars
import numpy as np
import sys
import os
import matplotlib.pyplot as plt

# TODO: plot the penetration as a function of history size...

nodes = range(0,16) # 0..15
penetration = {}

for i in range(1, len(sys.argv)):
    name = sys.argv[i]
    data = name.split("_")
    p = float(data[1])
    f = float(data[2])
    h = float(data[3])

    # penetration[(p,f)] = []

    with open(i,"r") as fh:
        pass

for n in nodes:
    size_data[n] = 0
    delay_data[n] = 0
    total[n] = 0

with open(sys.argv[1], "r") as fh:
    for line in fh.readlines():
        line = line.strip().split("\t")
        nid = int(line[0])
        # tt = int(line[1])
        delay = float(line[1])
        size = float(line[2])

        if nid in nodes:
            size_data[nid] += delay
            delay_data[nid] += size
            total[nid] += 1

sizes = map(lambda key : (size_data[key]) / total[nid], size_data)
delays = map(lambda key : (float(delay_data[key] / total[nid]) / float(1000000)), delay_data)

# sizes = []
# delays = []
#
# for i in range(160, 190):
#     sizes.append(size_data[i])
#     delays.append(delay_data[i])

print sizes, len(sizes)
print delays, len(delays)

# print np.arange(10)
ind = np.array(nodes)  # the x locations for the groups
# width = 0.35       # the width of the bars

width = 1.0
fig, ax = plt.subplots()
# size_rects = ax.bar(ind, sizes, width, color='r')
delay_rects = ax.bar(ind, delays, width, color='r')

# womenMeans = (25, 32, 34, 20, 25)
# womenStd = (3, 5, 2, 3, 3)
# rects2 = ax.bar(ind + width, womenMeans, width, color='y', yerr=womenStd)

# add some text for labels, title and axes ticks
ax.set_ylabel('Delay [ms]')
ax.set_title('AT&T Delay Overhead')
ax.set_xticks(ind) # + width)
ax.set_xticklabels((map(lambda n : str(n), nodes)))

# ax.legend((size_rects, delay_rects), ('History Size', 'Delay'))
# ax.legend(delay_rects, 'Delay [ms]')

# def autolabel(rects):
#     # attach some text labels
#     for rect in rects:
#         height = rect.get_height()
#         ax.text(rect.get_x() + rect.get_width()/2., 1.05*height,
#                 '%d' % int(height),
#                 ha='center', va='bottom')
#
# autolabel(rects1)
# autolabel(rects2)

plt.show()
