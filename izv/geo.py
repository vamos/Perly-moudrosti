#!/usr/bin/python3.8
# coding=utf-8
# IZV 2020 part 3.1 / xdvora2t

import pandas as pd
import geopandas
import matplotlib.pyplot as plt
import contextily
import sklearn.cluster
import numpy as np


def make_geo(df: pd.DataFrame) -> geopandas.GeoDataFrame:
    """ Konvertovani dataframe do geopandas.GeoDataFrame se spravnym kodovani"""

    # create GeoDataFrame
    df = df.loc[(df['region'] == 'JHM')]
    # drop invalid coordinate
    df = df.loc[(df['d'] >= -700000.00)]
    # create GeoDataFrame
    gdf = geopandas.GeoDataFrame(
        df, geometry=geopandas.points_from_xy(df["d"], df["e"]),
        crs="EPSG:5514")
    return gdf

def plot_geo(gdf: geopandas.GeoDataFrame, fig_location: str = None,
             show_figure: bool = False):
    """ Vykresleni grafu s dvemi podgrafy podle lokality nehody """

    # init figure
    fig, ax = plt.subplots(1, 2, figsize=(10, 5), sharex=True, sharey=True)
    # use sharp labels with CRS WGS84
    gdf = gdf.to_crs("epsg:3857")
    # plot accidents in town
    gdf[gdf['p5a'] == 1].plot(ax=ax[0], markersize=1, color='orange')
    # plot accidents out of town
    gdf[gdf['p5a'] == 2].plot(ax=ax[1], markersize=1, color='black')
    for a in ax:
        # insert background map
        contextily.add_basemap(a, crs=gdf.crs.to_string(), source=contextily.providers.Stamen.TonerLite)
        a.set_title("Nehody v Jihomoravském kraji (v obci)")
        # remove axis values
        a.axes.xaxis.set_visible(False)
        a.axes.yaxis.set_visible(False)
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

def plot_cluster(gdf: geopandas.GeoDataFrame, fig_location: str = None,
                 show_figure: bool = False):
    """ Vykresleni grafu s lokalitou vsech nehod v kraji shlukovanych do clusteru """

    # change crs
    gdf = gdf.to_crs("epsg:3857")
    # get coortinates for clustering model
    coords = np.dstack([gdf.geometry.x, gdf.geometry.y]).reshape(-1, 2)
    # make clustering model
    model = sklearn.cluster.MiniBatchKMeans(n_clusters=15).fit(coords)

    gdf2 = gdf.copy()
    # insert cluster type for each row/accident into dataframe
    gdf2['cluster'] = model.labels_

    # get coordinates for final points
    gdf2 = gdf2.dissolve(by='cluster', aggfunc={"p1": "count"}).rename(columns={'p1': "Pocet"})
    gdf_coords = geopandas.GeoDataFrame(geometry=geopandas.points_from_xy(model.cluster_centers_[:, 0],model.cluster_centers_[:, 1]))
    gdf3 = gdf2.merge(gdf_coords,left_on='cluster', right_index=True).set_geometry("geometry_y")

    # init figure
    plt.figure(figsize=(10, 5))
    ax = plt.gca()
    # plot all accidents
    gdf.plot(ax=ax, markersize=0.3, color='gray')
    # plot clusetrs
    gdf3.plot(ax=ax, markersize=gdf3['Pocet']/10, alpha=0.5, column='Pocet', legend=True)
    # insert background map
    contextily.add_basemap(ax, crs=gdf.crs.to_string(), source=contextily.providers.Stamen.TonerLite)
    ax.set_title("Nehody v Jihomoravském kraji")
    # remove axis values
    ax.axes.xaxis.set_visible(False)
    ax.axes.yaxis.set_visible(False)
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
    # zde muzete delat libovolne modifikace
    gdf = make_geo(pd.read_pickle("accidents.pkl.gz"))
    plot_geo(gdf, "geo1.png", False)
    plot_cluster(gdf, "geo2.png", False)
