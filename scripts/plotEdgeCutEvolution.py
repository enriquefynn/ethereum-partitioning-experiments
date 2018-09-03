#!/usr/bin/env python
import sys
import datetime
import matplotlib
import os
if 'SAVEGRAPH' in os.environ:
    matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import numpy as np


def set_ax_to_date(ax, timestamps):
    n_nodes_timestamps = map(datetime.datetime.fromtimestamp, timestamps)
    n_nodes_timestamps = mdates.date2num(n_nodes_timestamps)
    hfmt = mdates.DateFormatter('%m.%y')
    ax.xaxis.set_major_locator(mdates.MonthLocator())
    ax.xaxis.set_major_formatter(hfmt)
    return n_nodes_timestamps


def plot_tps_from_calls(ax, timestamps, calls, r_timestamps):
    tps = []
    for idx in range(1, len(calls)):
        tps.append(float(calls[idx]) /
                   float(r_timestamps[idx] - r_timestamps[idx - 1]))
    return ax.plot(timestamps, tps, color='#1f77b4')


def plot_cross_calls(ax, timestamps, cross_calls, non_cross_calls):
    percentage = []
    for idx in range(len(cross_calls)):
        percentage.append(
            (float(cross_calls[idx]) / float(cross_calls[idx] + non_cross_calls[idx])))
    print np.average(percentage)
    return ax.plot(timestamps, percentage, color='#1f77b4')


def plot_percentage_nodes_to_move(ax, timestamps, nodes_to_move, total):
    percentage = []
    for idx in range(len(nodes_to_move)):
        if total[idx] == 0:
            percentage.append(0)
            continue
        percentage.append((float(nodes_to_move[idx]) / float(total[idx])))
    if len(percentage) == 0:
        return None
    return ax.plot(timestamps, percentage, 'x', mew=2, markersize=6)[0]


def plot_edge_cut(ax, timestamps, total_edges, edges_cut):
    percentage = []
    for idx in range(len(edges_cut)):
        if total_edges[idx] == 0:
            percentage.append(0)
            continue
        percentage.append((float(edges_cut[idx]) / float(total_edges[idx])))
    return ax.plot(timestamps, percentage, '--', color='black')


def plot_balance(ax, timestamps, balances):
    bal = []
    for balance in balances:
        if sum(balance) == 0:
            bal.append(0)
            continue
        balance_coef = max(balance) / \
            (float(sum(balance)) / len(balance))
        # bal.append((balance_coef - 1.) / (len(balance) - 1))
        bal.append(balance_coef)
    return ax.plot(timestamps, bal, '--', color='black')


def plot_tx_per_partition(ax, timestamps, txs_per_partition):
    txs = []
    for tx_per_partition in txs_per_partition:
        tx_coef = max(tx_per_partition) / \
            (float(sum(tx_per_partition)) / len(tx_per_partition))
        txs.append(tx_coef)
    return ax.plot(timestamps, txs, color='#1f77b4')


def plotNodesEdgesEvolution(cross_calls, non_cross_calls, repartitions, timestamps,
                            balance, tx_per_partition, edges, edges_cut):
    # title = sys.argv[1].split('/')[-1]

    # fig = plt.figure(figsize=(12, 6))
    fig = plt.figure(figsize=(20, 6))
    ax_edge_cut = fig.add_subplot(211)
    ax_edge_cut.set_ylim(0, 1)
    ax_balance = fig.add_subplot(212)

    # ax_tx_load.set_title(title[:-4])
    ax_edge_cut.get_xaxis().set_visible(False)
    ax_balance.set_xlabel('date: Month.Year')
    # ax_tx_load.set_xlabel('date: Month.Year')
    # ax.set_ylabel('# calls')
    # ax_tx_load.set_ylabel('# calls')

    set_ax_to_date(ax_balance, timestamps)
    n_nodes_timestamps = set_ax_to_date(ax_edge_cut, timestamps)

    partitioning_timestamps = map(
        datetime.datetime.fromtimestamp, repartitions['timestamps'])
    partitioning_timestamps = mdates.date2num(partitioning_timestamps)

    # n_nodes_timestamps = n_nodes_timestamps[1:]

    # Plot Calls
    # cross_calls_line, = plot_tps_from_calls(ax, n_nodes_timestamps, cross_calls, timestamps)
    cross_calls_line, = plot_cross_calls(
        ax_edge_cut, n_nodes_timestamps, cross_calls, non_cross_calls)

    # non_cross_calls = get_tps_from_calls(non_cross_calls, timestamps)
    # non_cross_calls_line, = ax.plot(n_nodes_timestamps, non_cross_calls)

    # Plot Repartitioning
    for repart_ts in partitioning_timestamps:
        last_repart_line = ax_balance.axvline(
            x=repart_ts, color='red', ls='--', lw=0.8)
        ax_edge_cut.axvline(
            x=repart_ts, color='red', ls='--', lw=0.8)

    # Plot percentage moves
    percentage_move_line = plot_percentage_nodes_to_move(ax_balance, partitioning_timestamps,
                                                         repartitions['nodes_to_move'],
                                                         repartitions['n_vertices'])

    # Plot edge cut & balance
    edge_cut_line, = plot_edge_cut(
        ax_edge_cut, n_nodes_timestamps, edges_cut, edges)
    partition_load_line, = plot_tx_per_partition(
        ax_balance, n_nodes_timestamps, tx_per_partition)
    balance_line, = plot_balance(ax_balance, n_nodes_timestamps, balance)
    # Labeling
    label_lines = [cross_calls_line, edge_cut_line]
    label_names = ['Dynamic edge-cut', 'Static edge-cut']
    # if percentage_move_line != None:
    #     label_lines.append(percentage_move_line)
    #     label_names.append('Moves')
    # if len(partitioning_timestamps) != 0:
    # label_lines.append(last_repart_line)
    # label_names.append('Repartition')

    legend = ax_edge_cut.legend(label_lines, label_names, ncol=3)
    legend_tx_load = ax_balance.legend(
        [partition_load_line, balance_line], 
        ['Dynamic balance', 'Static balance'], 
        ncol=3, 
        bbox_to_anchor=(0.704, 0.94))

    legend_tx_load.get_frame().set_alpha(1)
    legend.get_frame().set_alpha(1)

    if 'SAVEGRAPH' in os.environ:
        fig.savefig(sys.argv[1][:-3] + 'pdf', bbox_inches='tight')
    else:
        plt.show()


if __name__ == '__main__':
    with open(sys.argv[1], 'r') as percentage_cross_file:
        cross_calls = []
        non_cross_calls = []
        balance = []
        tx_per_partition = []
        edges = []
        edges_cut = []

        repartitions = {'timestamps': [], 'nodes_to_move': [], 'n_vertices': [],
                        'n_edges': [], 'nodes_to_move': [], 'edges_cut': [], 'balance': []}
        timestamps = []
        line_n = 0

        for line in percentage_cross_file:
            line_n += 1
            line = line.split()
            value = line[0]
            args = map(lambda i: int(i), line[1:])
            # if value == 'POINT' and args[2] < 1472741238:
            #     continue
            # if value == 'REPARTITION' and args[0] < 1472741238:
            #     continue
            if value == 'POINT':
                cross_calls.append(args[0])
                non_cross_calls.append(args[1])
                timestamps.append(args[2])
                edges_cut.append(args[3])
                edges.append(args[4])
                balance.append(args[5::2])
                tx_per_partition.append(args[6::2])
            elif value == 'REPARTITION':
                repartitions['timestamps'].append(args[0])
                repartitions['n_vertices'].append(args[1])
                repartitions['n_edges'].append(args[2])
                repartitions['nodes_to_move'].append(args[3])
                repartitions['edges_cut'].append(args[4])
                repartitions['balance'].append(args[5:])
        plotNodesEdgesEvolution(cross_calls, non_cross_calls,
                                repartitions, timestamps, balance, tx_per_partition, edges, edges_cut)
