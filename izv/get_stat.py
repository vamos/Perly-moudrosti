"""get_stat.py: Plots last 5 years car accident data from czech regions."""

__author__      = "Libor Dvoracek"
__email__       = "xdvora2t@stud.fit.vutbr.cz"

from download import DataDownloader
from download import regions_dict
import matplotlib.pyplot as plt                     #scatter plot
import numpy as np
import argparse                                     #argument parser
import os


def arg_parser():
    parser = argparse.ArgumentParser(description='Plot stats')
    parser.add_argument("-f", "--fig_location", help="")
    parser.add_argument("-s", "--show_figure", help="", action="store_true")
    return parser.parse_args()


def aggregate_data(data_source):
    """Returns list of region abbreviations, and dict of year(s):values"""
    temp_list = []
    region_year_list =[]
    plot_values = {}
    # get list of region abbreviations
    plot_regions = np.unique(data_source[1][64])
    # create array of YYYY-REG
    for date, region in zip(data_source[1][3], data_source[1][64]):
        region_year_list.append("{}-{}".format(date.year, region))
    region_year_list= np.array(region_year_list)
    # count unique values for each tuple (year, region)
    uniq_regions, cnt_elements = np.unique(region_year_list, return_counts=True)
    #create list of ['year', 'region', 'count']
    for region_year,count in zip(uniq_regions, cnt_elements):
        temp_list.append(region_year +  "-" + str(count))
    temp_list = [item.split('-',2) for item in temp_list]
    # create output dict: keys=years, values=counts
    for item in temp_list:
        if item[0] in plot_values:
            plot_values[item[0]].append(int(item[2]))
        else:
            plot_values[item[0]] = [int(item[2])]
    return plot_regions, plot_values


def plot_stat(data_source, fig_location = None, show_figure = False):
    """Plot data for regions given in as get_list function arguments."""
    # prepare data for ploting
    regs, vals = aggregate_data(data_source)
    # make figure
    fig = plt.figure(constrained_layout=True, figsize=(8.27,11.69))
    axes = (
        fig.add_gridspec(ncols=1, nrows=5).subplots()
    )
    #start plot from this year
    starting_year = 2016
    # describe all axes
    for i in range(5):
        current_year = str(starting_year + i)
        axes[i].bar(regs, vals[current_year], color='black', zorder=3)
        axes[i].set_title(current_year)
        axes[i].set_ylabel('Pocet nehod')
        axes[i].grid(axis="y", color="grey", linewidth=1,
                    linestyle=":", zorder=0)
        axes[i].set_ylim(0,25000)
        # add label above each bar
        relative_order = []
        sortedVals = sorted(vals[current_year], reverse=True)
        relative_order = [sortedVals.index(i) for i in vals[current_year]]
        relative_order = [x+1 for x in relative_order]
        rects = axes[i].patches
        for rect, label in zip(rects, relative_order):
            height = rect.get_height()
            axes[i].text(rect.get_x() + rect.get_width() / 2, height + 5, label,
            ha='center', va='bottom', color='red', fontweight='bold')
    #plot data and/or save to file
    if fig_location:
        folder = fig_location.split('/')
        # folder specified
        if len(folder) > 1:
            # create if not exists
            if not os.path.exists(folder[0]):
                os.makedirs(folder[0])
        plt.savefig(str(fig_location))
    if show_figure:
        plt.show()
    exit(0)


if __name__ == "__main__":

    args = arg_parser()
    # show figure argument
    show_fig = False
    if args.show_figure:
        show_fig = True

    data_source = DataDownloader().get_list()

    if args.fig_location:
        plot_stat(data_source, args.fig_location, show_fig)
    else:
        plot_stat(data_source, None, show_fig)
