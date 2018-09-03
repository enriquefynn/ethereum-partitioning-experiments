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


def adjacent_values(vals, q1, q3):
    upper_adjacent_value = q3 + (q3 - q1) * 1.5
    upper_adjacent_value = np.clip(upper_adjacent_value, q3, vals[-1])

    lower_adjacent_value = q1 - (q3 - q1) * 1.5
    lower_adjacent_value = np.clip(lower_adjacent_value, vals[0], q1)
    return lower_adjacent_value, upper_adjacent_value


def set_color(bp):
    for pc in bp['bodies']:
        pc.set_facecolor('black')

    for partname in ('cbars', 'cmins', 'cmaxes', 'cmedians'):
        vp = bp[partname]
        vp.set_edgecolor('black')
        vp.set_linewidth(1)


class TextHandler(HandlerBase):
    def create_artists(self, legend, orig_handle, xdescent, ydescent,
                       width, height, fontsize, trans):
        h = copy.copy(orig_handle)
        h.set_position((width / 1.1, height / 3.))
        h.set_transform(trans)
        h.set_ha("center")
        h.set_va("center")
        fp = orig_handle.get_font_properties().copy()
        fp.set_size(fontsize)
        # uncomment the following line,
        # if legend symbol should have the same size as in the plot
        h.set_font_properties(fp)
        return [h]


def draw_legend(ax, legends):
    i = 1
    handles = []
    for _ in legends:
        handles.append(ax.text(x=0, y=0, s='${}$'.format(i)))
        i += 1

    handlermap = {type(handles[0]): TextHandler()}
    ax.legend(handles, legends, handler_map=handlermap,  ncol=2, loc=1, framealpha=1., fontsize='small')
    for handle in handles:
        handle.set_visible(False)


def set_box_numbers(ax, bp, legend, partitions=2):
    i = 0
    for _ in legend:
        # x, y = bp['whiskers'][2 * i + 1].get_xydata()[1]
        x, y = bp['cmaxes'].get_segments()[i][1]
        # y += 0.03 * (partitions / 2.)
        txt = ax.text(int(x), y, '{}'.format(
            legend[i]), horizontalalignment='center')
        txt_pos = txt.get_position()
        txt_pos = (txt_pos[0], txt_pos[1] + 0.06)
        print txt_pos
        txt.set_position(txt_pos)
        # setp(bp['boxes'][i])
        # setp(bp['caps'][2 * i], color=color['color'])
        # setp(bp['caps'][2 * i + 1], color=color['color'])
        # setp(bp['whiskers'][2 * i], color=color['color'])
        # setp(bp['whiskers'][2 * i + 1], color=color['color'])
        # # setp(bp['fliers'][2 * i], color=color)
        # # setp(bp['fliers'][2 * i + 1], color=color)
        # setp(bp['medians'][i], color=color['color'])
        i += 1

def set_number(ax, plot, legend, partitions=2, idx=0):
    xy = plot[0].get_xydata()
    i = 0
    for (x, y) in xy:
        ax.text(x, y, '{}\n'.format(
            legend[i]), horizontalalignment='center')
        # txt_pos = txt.get_position()
        # txt_pos = (txt_pos[0], txt_pos[1] + 0.06)
        # txt.set_position(txt_pos)
        i+=1

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


def get_percentage_period(experiment, exp_type, **kwargs):
    ret = []
    current_period = experiment['calls']['timestamps'][0]

    if exp_type == 'call':
        for i in range(len(experiment['calls']['cross_calls'])):
            total = float(experiment['calls']['cross_calls'][i]
                          + experiment['calls']['non_cross_calls'][i])

            if experiment['calls']['timestamps'][i] < current_period + kwargs['period']:
                ret.append(experiment['calls']['cross_calls'][i] / total)
            else:
                ret.append(experiment['calls']['cross_calls'][i] / total)
                # ret.append(experiment['calls']['cross_calls'][i] / total)
                # ret.append(experiment['calls']['timestamps'][i])
                yield {
                    'begin_ts': current_period,
                    'end_ts': experiment['calls']['timestamps'][i],
                    'data': ret
                }
                ret = []
                current_period = experiment['calls']['timestamps'][i]

        if len(ret) != 0:
            yield {
                'begin_ts': current_period,
                'end_ts': experiment['calls']['timestamps'][i],
                'data': ret
            }
    elif exp_type == 'load':
        def get_load():
            return max(experiment['calls']['tx_per_partition'][i]) / (np.mean(experiment['calls']['tx_per_partition'][i]))

        for i in range(len(experiment['calls']['tx_per_partition'])):

            if experiment['calls']['timestamps'][i] < current_period + kwargs['period']:
                ret.append(get_load())
            else:
                ret.append(get_load())
                # ret.append(experiment['calls']['cross_calls'][i] / total)
                # ret.append(experiment['calls']['timestamps'][i])
                yield {
                    'begin_ts': current_period,
                    'end_ts': experiment['calls']['timestamps'][i],
                    'data': ret
                }
                ret = []
                current_period = experiment['calls']['timestamps'][i]

        if len(ret) != 0:
            yield {
                'begin_ts': current_period,
                'end_ts': experiment['calls']['timestamps'][i],
                'data': ret
            }

    elif exp_type == 'moves':
        repartitions = experiment['repartitions']['timestamps']
        if len(repartitions) == 0:
            raise 'No data'

        for i in range(len(experiment['repartitions']['nodes_to_move'])):
            moves = experiment['repartitions']['nodes_to_move'][i]
            if experiment['repartitions']['timestamps'][i] < current_period + kwargs['period']:
                ret.append(moves)
            else:
                ret.append(moves)
                # ret.append(experiment['calls']['cross_calls'][i] / total)
                # ret.append(experiment['calls']['timestamps'][i])
                yield {
                    'begin_ts': current_period,
                    'end_ts': experiment['repartitions']['timestamps'][i],
                    'data': ret
                }
                ret = []
                current_period = experiment['repartitions']['timestamps'][i]

        if len(ret) != 0:
            yield {
                'begin_ts': current_period,
                'end_ts': experiment['repartitions']['timestamps'][i],
                'data': ret
            }


def get_experiment(exp_type, **kwargs):
    all_experiments = []
    for experiment in experiments:
        percentages = []
        for percentage_period in get_percentage_period(experiment, exp_type, **kwargs):
            # print percentage_period
            percentages.append(percentage_period)
        if percentages[0] == None:
            all_experiments.append({'name': experiment['name'], 'data': [{
                'begin_ts': int(kwargs['begin_ts']),
                'end_ts': int(kwargs['begin_ts']),
                'data':  [0 for i in range(42)]
            }]})
        else:
            all_experiments.append(
                {'name': experiment['name'], 'data': percentages})
    return all_experiments


def plot_boxes(ax, experiments, GAP_LIMIT=2, partitions=2):
    gap = 1
    ticks = []
    tick_names = []
    vertical_lines = []

    for experiment in experiments[0]['data']:
        begin_ts, end_ts = experiment['begin_ts'], experiment['end_ts']
        time_start, time_end = time.strftime(
            '%m.%y', time.localtime(begin_ts)), time.strftime('%m.%y', time.localtime(end_ts))
        tick_names.append('{} - {}'.format(time_start, time_end))

    for experiment_data_idx in range(len(experiments[0]['data'])):
        box_experiments = []
        for experiment in experiments:
            box_experiments.append(
                experiment['data'][experiment_data_idx]['data'])
        data_points = [i + gap for i in range(len(box_experiments))]
        ticks.append(np.mean(data_points))

        # bp = ax.boxplot(box_experiments, 0, '', positions=data_points,
        #                 whis='range', widths=0.4)
        quartiles = map(lambda exp: np.percentile(
            exp, [25, 75]), box_experiments)

        # whiskers = np.array([adjacent_values(sorted_array, q1, q3)
        #     for sorted_array, q1, q3 in zip(box_experiments, quartile1, quartile3)])
        # whiskersMin, whiskersMax = whiskers[:, 0], whiskers[:, 1]

        # ax.scatter(data_points, medians, marker='x', color='black', s=20, zorder=3)
        ax.vlines(data_points, map(lambda q: q[0], quartiles), map(
            lambda q: q[1], quartiles), linestyle='-', lw=6)

        # ax.vlines(data_points, whiskersMin, whiskersMax, linestyle='-', lw=5)
        bp = ax.violinplot(box_experiments, data_points, showmedians=True)
        set_color(bp)

        set_box_numbers(
            ax, bp, [i + 1 for i in range(len(experiments))], partitions=partitions)
        gap += len(experiments[0]['data']) + GAP_LIMIT
        vertical_lines.append((data_points[-1] + gap) / 2.)
    if len(vertical_lines) != 0:
        for vl in vertical_lines[:-1]:
            ax.axvline(x=vl, ls='--')
        ax.set_xlim(0, vertical_lines[-1])

    # draw_legend(ax, map(lambda exp: exp['name'].split('_')[7], experiments))

    ax.set_xticks(ticks)
    ax.set_xticklabels(tick_names)


def plot_point(ax, experiments, GAP_LIMIT=2):
    gap = 1
    ticks = []
    tick_names = []
    vertical_lines = []

    for experiment in experiments[0]['data']:
        begin_ts, end_ts = experiment['begin_ts'], experiment['end_ts']
        time_start, time_end = time.strftime(
            '%m.%y', time.localtime(begin_ts)), time.strftime('%m.%y', time.localtime(end_ts))
        tick_names.append('{} - {}'.format(time_start, time_end))

    for experiment_data_idx in range(len(experiments[0]['data'])):
        box_experiments = []
        for experiment in experiments:
            box_experiments.append(
                experiment['data'][experiment_data_idx]['data'])
        data_points = [i + gap for i in range(len(box_experiments))]
        ticks.append(np.mean(data_points))

        box_experiments = map(lambda i: sum(i) + 1, box_experiments)
        point_plot = ax.plot(data_points, box_experiments, 'x', color='black')
        set_number(ax, point_plot, [i + 1 for i in range(len(experiments))], idx=experiment_data_idx)
        # set_color(bp)

        # set_box_numbers(
        #     ax, bp, [i + 1 for i in range(len(experiments))])
        gap += len(experiments[0]['data']) + GAP_LIMIT
        vertical_lines.append((data_points[-1] + gap) / 2.)
    if len(vertical_lines) != 0:
        for vl in vertical_lines[:-1]:
            ax.axvline(x=vl, ls='--')
        ax.set_xlim(0, vertical_lines[-1])

    # draw_legend(ax, map(lambda exp: exp['name'].split('_')[7], experiments))

    ax.set_xticks(ticks)
    ax.set_xticklabels(tick_names)


if __name__ == '__main__':
    BEGIN_TIME = '01.01.2017'
    END_TIME = '01.01.2018'
    PARTITIONS = 8

    timestamps = {
        'begin_timestamp': calendar.timegm(time.strptime(BEGIN_TIME, "%d.%m.%Y")),
        'end_timestamp': calendar.timegm(time.strptime(END_TIME, "%d.%m.%Y"))
    }
    experiments = []
    for arg in sys.argv[1:]:
        with open(arg, 'r') as experiment_file:
            exp_name = arg.split('/')[-1][:-4]
            experiment = parse_experiment(
                experiment_file, **timestamps)
            exp_calls, exp_repartitions = experiment
            experiments.append({'name': exp_name,
                                'calls': exp_calls,
                                'repartitions': exp_repartitions
                                })
    fig = plt.figure(figsize=(20, 6))
    ax_cross_partition = fig.add_subplot(311)
    ax_tx_load = fig.add_subplot(312)
    ax_moves = fig.add_subplot(313)
    ax_moves.set_yscale('log')
    ax_moves.set_ylim([1,1e10])

    ax_cross_partition.set_title('Dynamic edge-cut')
    ax_tx_load.set_title('Dynamic balance')
    ax_moves.set_title('Moves')

    ax_cross_partition.set_ylim([-0.02, 1.2])
    # ax_tx_load.set_ylim([0.9, 2.3])
    ax_tx_load.set_ylim([0.9, 8.9])

    calls_exp = get_experiment(
        'call', begin_ts=timestamps['begin_timestamp'], period=3 * 30 * 24 * 60 * 60)
    load_exp = get_experiment(
        'load', begin_ts=timestamps['begin_timestamp'], period=3 * 30 * 24 * 60 * 60)
    moves_exp = get_experiment(
        'moves', begin_ts=timestamps['begin_timestamp'], period=3 * 30 * 24 * 60 * 60)

    plot_boxes(ax_cross_partition, calls_exp)
    plot_boxes(ax_tx_load, load_exp, partitions=PARTITIONS)
    # plot_boxes(ax_moves, moves_exp, partitions=PARTITIONS)
    plot_point(ax_moves, moves_exp)

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

    legends = map(lambda exp: exp['name'].split('_')[7], experiments)
    draw_legend(ax_cross_partition, legends)

    if 'SAVEGRAPH' not in os.environ:
        plt.show()
    else:
        fig.savefig('box_plot.pdf', bbox_inches='tight')
