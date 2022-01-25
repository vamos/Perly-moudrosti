# IZV 2020 part3.3 / xdvora2t

import numpy as np
import pandas as pd
import seaborn as sns
from matplotlib import pyplot as plt
import matplotlib as mpl
from matplotlib.patches import Patch


def get_data():
    """ Create and format DataFrame."""

    df = pd.read_pickle("accidents.pkl.gz")
    df['rok nehody']= pd.DatetimeIndex(df['p2a']).year
    # filter only cars
    df.loc[(df['p44'] == 4) | (df['p44'] == 3)]
    # filter invalid year
    df = df[df.p47 != -1]
    # get exact year of car manufacture
    df.loc[df['p47'] > 20, 'p47'] =  df['p47'] + 1900
    df.loc[df['p47'] <= 20, 'p47'] =  df['p47'] + 2000
    # create categories for causes
    priciny = ['nezaviněná řídičem',
                'nepřiměřená rychlost',
                'nesprávné předjíždění',
                'nedání přednost v jízdě',
                'nesprávný způsob jízdy',
                'technická závada vozidla'
                ]
    df['pricina'] = pd.cut(df['p12'],
                              bins=[0,150,250,350,450,550,650],
                              labels=priciny)

    return df

def plot_histo(df: pd.DataFrame):
    """ Plot histogram: Accident causes by year of car manufacture."""

    # compute minority data
    count_tech_z = df['pricina'].value_counts()
    res1 = count_tech_z['technická závada vozidla'] / (count_tech_z.sum() / 100)
    print('Podíl nehod zaviněných technickou závadou vozidla: {:.3f}%'.format(res1))
    count_predjizdeni = df['pricina'].value_counts()
    res2 = count_predjizdeni['nesprávné předjíždění'] / (count_predjizdeni.sum() / 100)
    print('Podíl nehod zaviněných nesprávným předjížděním:    {:.3f}%'.format(res2))

    df = df.loc[(df['pricina'] != 'technická závada vozidla')]
    df = df.loc[(df['pricina'] != 'nesprávné předjíždění')]
    df['pricina'] = df.pricina.cat.remove_unused_categories()
    print("")

    # filter cars made before 1980 / minority data
    df = df[df.p47 > 1980]

    # plot histogram
    sns.set_theme(style="ticks")
    f, ax = plt.subplots(figsize=(7, 5))
    ax.set(xlim=(1980, 2020))
    ax.set(ylim=(0, 30_000))
    sns.despine(f)

    # create custom palette
    colors = ['#707070', '#FF0000', '#C7C7C8', '#E9E9E9']
    sns.set_palette(sns.color_palette(colors))

    sns.histplot(
        df,
        x='p47', hue='pricina',
        multiple="stack",
        edgecolor=".3",
        linewidth=1.5,
        log_scale=False,
        bins= 40
        )
    # set axis properties
    ax.xaxis.set_major_formatter(mpl.ticker.ScalarFormatter())
    ax.set_title('nehody v ČR v období 1/2016 - 9/2020')
    ax.set_xlabel('rok vyroby vozidla')
    ax.set_xticks([1980, 1990, 2000, 2010, 2020])
    ax.set_ylabel('pocet nehod')
    # save into file
    plt.savefig(str('priciny_rok_vyr.PNG'))

def tab_deaths(df: pd.DataFrame):
    """ Prints table data and result."""

    pd.options.display.float_format = '{:,.2f}%'.format
    # filter only relevant features
    df = df[['p1', 'p12', 'p13a', 'p44', 'p47', 'pricina']]

    # create category for old and new vehicles
    df = df.assign(oldnew='old car')
    df.loc[df['p47'] > 2010, 'oldnew'] = 'new car'
    # all accidents for new and old cars
    new_all_acc = df['oldnew'].value_counts()

    # table 1
    df_fast = df[df.pricina == 'nepřiměřená rychlost']
    new_fast = df_fast['oldnew'].value_counts()
    tab_fast = pd.concat([new_all_acc, new_fast],
                            axis=1,
                            sort=False
                            )
    tab_fast.columns = ['all accidents', 'cause=speed']
    tab_fast['ratio[%]'] = tab_fast['cause=speed'] / (tab_fast['all accidents'] / 100 )
    print('Tabulka 1: počet nehod způsobených vysokou rychlostí')
    print(tab_fast)

    # table 2
    # only accident with death
    df = df[df.p13a != 0]
    df_deadly_acc = df['oldnew'].value_counts()

    tab_deaths = pd.concat([new_all_acc, df_deadly_acc],
                            axis=1,
                            sort=False
                            )
    tab_deaths.columns = ['all accidents', 'deaths']
    print('')
    print('Tabulka 2: Počet úmrtí')
    print(tab_deaths)

    # get percentage of deadly accidents
    tab_deaths['ratio'] = tab_deaths['deaths'] / (tab_deaths['all accidents'] / 100 )
    old = tab_deaths.iloc[0]['ratio']
    new = tab_deaths.iloc[1]['ratio']
    diff = ( old / ( new / 100 )) - 100
    print('')
    print('Nárůst smrtelných nehod o {:.1f}%'.format(diff))


if __name__ == "__main__":
    df = get_data()
    plot_histo(df)
    tab_deaths(df)
