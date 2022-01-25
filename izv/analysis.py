#!/usr/bin/env python3.8
# coding=utf-8

# izv 2nd part / xdvora2t

from matplotlib.dates import DateFormatter
from matplotlib import pyplot as plt
import matplotlib.dates as mdates
import seaborn as sns
import pandas as pd
import numpy as np
import pickle
import gzip
import os

# Ukol 1: nacteni dat
def get_dataframe(filename: str, verbose: bool = False) -> pd.DataFrame:

    orig_df = pd.read_pickle(filename, compression='gzip')
    # make new DataFrame
    new_df = orig_df.copy(deep=True)
    # create column date
    new_df["date"] = pd.to_datetime(new_df['p2a'])
    # change datatype to category on selected columns
    new_df.iloc[:,1:3] = new_df.iloc[:, 1:3].astype('category')
    new_df.iloc[:,4:5] = new_df.iloc[:, 4:5].astype('category')
    new_df.iloc[:,6:13] = new_df.iloc[:, 6:13].astype('category')
    new_df.iloc[:,17:41] = new_df.iloc[:, 17:41].astype('category')
    new_df.iloc[:,58:64] = new_df.iloc[:, 58:64].astype('category')

    if verbose:
        orig_size = orig_df.memory_usage(deep=True).sum()
        print("orig_size={:.1f} MB".format(orig_size / 1_048_576))
        new_size = new_df.memory_usage(deep=True).sum()
        print("new_size={:.1f} MB".format(new_size / 1_048_576))
    return new_df

# Ukol 2: následky nehod v jednotlivých regionech
def plot_conseq(df: pd.DataFrame, fig_location: str = None,
                show_figure: bool = False):

    # sort regions by number of accidents, descending
    regions = df.region.value_counts().index
    # get info variables for plot
    all_deaths = df.groupby('region')['p13a'].sum()
    heavily_injured = df.groupby('region')['p13b'].sum()
    lightly_injured = df.groupby('region')['p13c'].sum()
    all_accidents = df.region.value_counts().sort_index()
    # make single table of them
    df_concat= pd.concat([all_deaths, heavily_injured, lightly_injured, all_accidents],
                            axis=1,
                            sort=False
                            )
    df_concat.rename(columns={'p13a':'Úmrtí',
                                'p13b':'Těžce zranění',
                                'p13c':'Lehce zranění',
                                'region':'Celkem nehod',
                                'values':'Počet'}, inplace=True)
    df_concat.reset_index(inplace=True)
    ax_titles = ['Úmrtí', 'Těžce zranění', 'Lehce zranění', 'Celkem nehod']
    df_concat = df_concat.melt(id_vars=['region'],
                                value_vars=ax_titles,
                                value_name='Počet')
    # plot data
    sns.set_style('darkgrid')
    g = sns.FacetGrid(
            df_concat,
            row='variable',
            palette='rocket',
            hue='variable',
            height=1.7,
            aspect=4,
            sharey=False
       )
    g = g.map(sns.barplot, 'region', 'Počet', order=regions, zorder=3)
#
    # set axes properties and titles
    axes = g.axes.flatten()
    for ax,title in zip(axes, ax_titles):
        ax.set_title(title)
        ax.grid(axis='y', color='white', linewidth=1, zorder=0)
##
#    # handle optional parameters
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
    plt.close()

# Ukol3: příčina nehody a škoda
def plot_damage(df: pd.DataFrame, fig_location: str = None,
                show_figure: bool = False):
    # create new dataframe
    df_damage = df.loc[(df['region'] == 'JHM')
                        | (df['region'] == 'PHA')
                        | (df['region'] == 'KVK')
                        | (df['region'] == 'VYS')].copy()
    # correct damage values ( was stated in hundreds )
    df_damage['p53'] = df_damage['p53'] * 100
    # set column names for "causes"
    priciny = ['nezaviněná řídičem',
                'nepřiměřená rychlost',
                'nesprávné předjíždění',
                'nedání přednost v jízdě',
                'nesprávný způsob jízdy',
                'technická závada vozidla'
                ]
    # set column names for "damage"
    skoda = ['< 50', '50 - 200', '200 - 500', '500 - 1 000', '> 1 000']
    # cut values to bins
    df_damage['pricina'] = pd.cut(df_damage['p12'],
                                    bins=[0,150,250,350,450,550,650],
                                    labels=priciny)
    df_damage['škoda'] = pd.cut(df_damage['p53'],
                                bins=[float('-inf'),50_000,200_000,500_000,1_000_000, float('inf')],
                                labels=skoda)
    # plot data
    sns.set_style('dark')
    sns.despine(left = True, bottom = True)
    d_plot = sns.catplot(
                data=df_damage,
                x='škoda',
                hue='pricina',
                col='region',
                col_wrap=2,
                kind='count',
                log=True,
                palette='colorblind',
                height=4,
                aspect=1,
                zorder=3
              )
#
    # change graph titles
    ax_titles = df_damage['region'].unique()
    axes = d_plot.axes.flatten()
    for ax,title in zip(axes, ax_titles):
        ax.set_title(title)
        ax.grid(axis='y', color='white', linewidth=1, zorder=0)
#
    # set labels
    axes[0].set_ylabel('Počet')
    axes[2].set_ylabel('Počet')
    axes[2].set_xlabel('Škoda (v tisících Kč)')
    axes[3].set_xlabel('Škoda (v tisících Kč)')
#
    # xaxis label out of document
    plt.gcf().subplots_adjust(bottom=0.07)
#
    # handle optional parameters
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
    plt.close()

# Ukol 4: povrch vozovky
def plot_surface(df: pd.DataFrame, fig_location: str = None,
                 show_figure: bool = False):

    # create new dataframe
    df_surface = df.loc[(df['region'] == 'JHM')
                        | (df['region'] == 'PHA')
                        | (df['region'] == 'KVK')
                        | (df['region'] == 'VYS')].copy()
    df_surface = df_surface[['p16', 'region', 'date']]
    df_surface['date'] = df_surface['date'].dt.to_period('M')
    df_surface['date'] = df_surface['date'].dt.to_timestamp('M').dt.strftime('%Y-%m')
    df_surface = pd.crosstab(index=[df_surface['region'], df_surface['date']], columns=df_surface['p16'])
    df_surface = df_surface.rename(columns={0: "jiný",
                                            1: "povrch suchy(neznečištěný)",
                                            2: 'povrch suchy(znečištěný)',
                                            3:'povrch mokrý',
                                            4:'na vozovce je bláto',
                                            5:'na vozovce je náledí, ujetý sníh(posypané)',
                                            6:'na vozovce je náledí, ujetý sníh(neposypané)',
                                            7:'na vozovce je rozlitý olej, nafta apod.',
                                            8:'souvislá sněhová vrstva, rozbředlý sníh',
                                            9:'náhlá změna svavu vozovky'
                                            })

    df_surface = df_surface.reset_index()
    df_surface = pd.melt(df_surface, ['date', 'region'])
    df_surface.rename(columns={'date': 'Datum', 'value': 'Počet nehod', 'p16': 'Stav vozovky'}, inplace=True)
    df_surface['Datum'] = pd.to_datetime(df_surface['Datum'])
    # plot data
    s_plot = sns.relplot(
                data=df_surface,
                x='Datum', y='Počet nehod',
                hue='Stav vozovky', col='region', col_wrap=2,
                kind='line', palette='colorblind',
                height=2, aspect=2.75, facet_kws=dict(sharex=False, sharey=False),
                zorder=3
            )
    # change graph titles
    ax_titles = df_surface['region'].unique()
    axes = s_plot.axes.flatten()
    for ax,title in zip(axes, ax_titles):
        ax.set_title(title)
        ax.grid(axis='y', color='white', linewidth=1, zorder=0)
        ax.grid(axis='x', color='white', linewidth=1, zorder=0)
    # xaxis label out of document
    plt.gcf().subplots_adjust(bottom=0.15)
#
    # handle optional parameters
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
    plt.close()

if __name__ == "__main__":
    pass
    # zde je ukazka pouziti
    df = get_dataframe("accidents.pkl.gz", verbose=True)
    plot_conseq(df, fig_location="01_nasledky.png", show_figure=False)
    plot_damage(df, "02_priciny.png", False)
    plot_surface(df, "03_stav.png", False)
