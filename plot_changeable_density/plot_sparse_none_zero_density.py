#! /usr/bin/env python
from _threading_local import local
from random import randint
import numpy as np
import pandas as pd
import scipy
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages as pdfpages

print("Using pandas {}".format(pd.__version__))

df = pd.read_csv('fulcrumsparse_none_zero.csv')
colors = ['red', 'blue', 'green', 'yellow', 'black', 'cyan', 'magenta', 'red',  'grey', 'brown' ]
def plot(args):
    plt.figure()
    for d in xrange(1, 10):
        dd = float(d) / 10
        density = df[df['density'] == dd]
        den = density[['time', 'nonezero']].copy()
        lbl = 'density '+ str(dd)
        plt.plot(den['time'], den['nonezero'], label=lbl, color=colors[d])
        plt.ylabel('Desity')
        plt.xlabel('Number of coded packet')
    plt.legend(loc='best', prop={"size": 8})
    plt.show()

if __name__ == '__main__':
    args = ['encoder', 'decoder']
    plot(args)