import numpy as np
import seaborn as sns
from matplotlib import pyplot as plt

def plot_boxenplot():
    """ x: kategorie, y:cisla"""
    sns.set_theme(style="whitegrid")

    diamonds = sns.load_dataset("diamonds")
    clarity_ranking = ["I1", "SI2", "SI1", "VS2", "VS1", "VVS2", "VVS1", "IF"]

    sns.boxenplot(x="clarity", y="carat",
                color="b", order=clarity_ranking,
                scale="linear", data=diamonds)
    print(diamonds)
    plt.show()

def plot_jointgrid():
    sns.set_theme(style="white")

    df = sns.load_dataset("penguins")

    g = sns.JointGrid(data=df, x="body_mass_g", y="bill_depth_mm", space=0)
    g.plot_joint(sns.kdeplot,
                fill=True, clip=((2200, 6800), (10, 25)),
                thresh=0, levels=100, cmap="rocket")
    g.plot_marginals(sns.histplot, color="#03051A", alpha=1, bins=25)
    print(df)
    plt.show()


def plot_jointplot():
    sns.set_theme(style="ticks")

    rs = np.random.RandomState(11)
    x = rs.gamma(2, size=1000)
    y = -.5 * x + rs.normal(size=1000)

    print(x)
    print(y)

    sns.jointplot(x=x, y=y, kind="hex", color="#4CB391")
    plt.show()


if __name__ == "__main__":
    #plot_boxenplot()
    plot_jointgrid()
    #plot_jointplot()
