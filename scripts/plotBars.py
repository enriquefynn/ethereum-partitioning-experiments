#!/usr/bin/env python
import sys
import numpy as np

import time
import datetime
import time
import calendar

import matplotlib
import copy
import os
if 'SAVEGRAPH' in os.environ:
    matplotlib.use('Agg')

from matplotlib.legend_handler import HandlerBase
import matplotlib.pyplot as plt
from pylab import setp
from cycler import cycler

color_cycle = cycler(color=['r', 'g', 'b', 'y', 'c'])


def parse_experiment(experiment_file, begin_timestamp=0, end_timestamp=float('inf')):
    calls = {
        'timestamps': [],
        'cross_calls': [],
        'non_cross_calls': [],
        'balance': [],
        'tx_per_partition': [],
        'edges': [],
        'edges_cut': []
    }
    repartitions = {'timestamps': [], 'nodes_to_move': [], 'n_vertices': [],
                    'n_edges': [], 'edges_cut': [], 'balance': []}
    for line in experiment_file:
        line = line.split()
        value = line[0]
        args = map(lambda i: int(i), line[1:])
        # Timestamp
        if value == 'POINT':
            if args[2] < begin_timestamp or args[2] > end_timestamp:
                continue
            calls['cross_calls'].append(args[0])
            calls['non_cross_calls'].append(args[1])
            calls['timestamps'].append(args[2])
            calls['edges_cut'].append(args[3])
            calls['edges'].append(args[4])
            calls['balance'].append(args[5::2])
            calls['tx_per_partition'].append(args[6::2])
        elif value == 'REPARTITION':
            if args[0] < begin_timestamp or args[0] > end_timestamp:
                continue
            repartitions['timestamps'].append(args[0])
            repartitions['n_vertices'].append(args[1])
            repartitions['n_edges'].append(args[2])
            repartitions['nodes_to_move'].append(args[3])
            repartitions['edges_cut'].append(args[4])
            repartitions['balance'].append(args[5:])
    return calls, repartitions


def get_experiments(experiment, exp_type, partitions=2):
    if exp_type == 'call':
        cross_calls = sum(experiment['calls']['cross_calls'])
        all_calls = cross_calls + sum(experiment['calls']['non_cross_calls'])
        return float(cross_calls) / all_calls

    elif exp_type == 'load':
        avg_loads = map(lambda i: max(i) / np.average(i),
                        experiment['calls']['tx_per_partition'])
        avg_load = np.average(avg_loads)

        return (avg_load - 1) / (p - 1)

    elif exp_type == 'moves':
        repartitions = experiment['repartitions']['timestamps']
        if len(repartitions) == 0:
            return 0 + 1
        return np.sum(experiment['repartitions']['nodes_to_move']) + 1


def plot_bars(ax, x, experiment, gap=0.20):
    plots = []
    plots.append(
        ax.bar(x, experiment[0], width=0.2, fill=False, align='center', label='2 partitions'))
    plots.append(ax.bar(x + gap, experiment[1], width=0.2,
                        fill=False, hatch='///', align='center', label='4 partitions'))
    plots.append(ax.bar(
        x + 2 * gap, experiment[2], width=0.2, fill=False, hatch='...', align='center', label='8 partitions'))
    return plots


if __name__ == '__main__':
    BEGIN_TIME = '01.06.2017'
    END_TIME = '01.01.2018'
    GAP = 0.3

    timestamps = {
        'begin_timestamp': calendar.timegm(time.strptime(BEGIN_TIME, "%d.%m.%Y")),
        'end_timestamp': calendar.timegm(time.strptime(END_TIME, "%d.%m.%Y"))
    }
    partition = 2
    experiments = {2: [], 4: [], 8: []}
    for arg in sys.argv[1:]:
        with open(arg, 'r') as experiment_file:
            exp_name = arg.split('/')[-1][:-4]
            experiment = parse_experiment(
                experiment_file, **timestamps)
            exp_calls, exp_repartitions = experiment
            experiments[partition].append({'name': exp_name,
                                           'calls': exp_calls,
                                           'repartitions': exp_repartitions
                                           })
        partition = (partition * 2) % 14

    fig = plt.figure(figsize=(20, 6))
    ax_cross_partition = fig.add_subplot(311)
    ax_tx_load = fig.add_subplot(312)
    ax_moves = fig.add_subplot(313)

    # ax_moves.set_yscale('log')
    # ax_moves.set_ylim([1, 1e])

    ax_cross_partition.set_ylim([0, 1])

    ax_cross_partition.tick_params(
        axis='x',          # changes apply to the x-axis
        which='both',      # both major and minor ticks are affected
        bottom='off',      # ticks along the bottom edge are off
        top='off',         # ticks along the top edge are off
        labelbottom='off')  # labels along the bottom edge are off
    ax_tx_load.tick_params(
        axis='x',          # changes apply to the x-axis
        which='both',      # both major and minor ticks are affected
        bottom='off',      # ticks along the bottom edge are off
        top='off',         # ticks along the top edge are off
        labelbottom='off')  # labels along the bottom edge are off

    # legends = ['Hashing', 'KL', 'METIS', 'P-METIS', 'TR-METIS']
    legends = ['Hashing', 'KL', 'METIS', 'P-METIS', 'TR-METIS']

    ax_moves.set_xticks([0.20, 1, 1.8, 2.6, 3.4])
    ax_moves.set_xticklabels(legends)

    for i in range(len(legends)):
        cross_partition = []
        tx_load = []
        moves = []
        for p in [2, 4, 8]:
            cross_partition.append(get_experiments(
                experiments[p][i], 'call', partitions=p))
            tx_load.append(get_experiments(
                experiments[p][i], 'load', partitions=p))
            moves.append(get_experiments(
                experiments[p][i], 'moves', partitions=p))
        plots = plot_bars(ax_cross_partition, i * 0.8, cross_partition)
        plot_bars(ax_tx_load, i * 0.8, tx_load)
        print moves
        plot_bars(ax_moves, i * 0.8, moves)


    ax_moves.set_yscale('log')
    ax_moves.set_ylim([1e6, 1e10])
    ax_cross_partition.legend(handles=plots)

    ax_cross_partition.set_ylabel('Dynamic edge-cut')
    ax_tx_load.set_ylabel('Dynamic balance')
    ax_moves.set_ylabel('Moves')

    if 'SAVEGRAPH' not in os.environ:
        plt.show()
    else:
        fig.savefig('bar_plot.pdf', bbox_inches='tight')
