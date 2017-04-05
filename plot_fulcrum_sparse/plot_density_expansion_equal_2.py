#! /usr/bin/env python
from _threading_local import local

import numpy as np
import pandas as pd
import scipy
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages as pdfpages

print("Using pandas {}".format(pd.__version__))


dffd = pd.read_csv('out_fulcrum_sparse.csv')
df = pd.read_csv('out_fulcrum.csv')

def plot(args):


    filename = "fulcrum_innerdecoder_expansion2_densities.pdf"
    with pdfpages(filename) as pdf:
        colors = ['red', 'blue', 'green', 'yellow', 'black', 'cyan', 'magenta', 'red', 'grey', 'brown']
        for type in args:
            y_scale = [20, 50, 100, 300, 600, 1000, 1500, 2000, 3000, 4000]

            df_fd = dffd[dffd['type'] == type]
            fulcrum = df[df['type'] == type]
            symbols = dffd['symbols']

            df_fd = df_fd[(df_fd['testcase'] == 'FulcrumInner') & (df_fd['expansion'] == 2)]
            df_fd = df_fd[['density', 'throughput', 'symbols']].copy()
            df_fd = df_fd.groupby(['density', 'symbols'])['throughput'].mean()
            df_fd = df_fd.unstack(level=0)

            fulcrum = fulcrum[(fulcrum['testcase'] == 'FulcrumInner') & (fulcrum['expansion'] == 2)]
            fulcrum = fulcrum[['throughput', 'symbols']].copy()
            fulcrum = fulcrum.groupby(['symbols'])['throughput'].mean()
            # fulcrum = fulcrum.unstack(level=0)

            plt.figure()

            for d in xrange(1, 10):
                dd = float(d) / 10
                # den = density[['time', 'nonezero']].copy()
                lbl = 'density = ' + str(dd)
                plt.plot(df_fd.index, df_fd[dd], label=lbl, color=colors[d])
            # fulcrum.plot(color='pink', linewidth=2, linestyle='--', label='Fulcrum')

            plt.ylabel('Throughput [Mbps]')
            plt.xlabel('Generation size')

            plt.yscale('log', linthreshy=10)
            plt.xscale('log', basex=2)
            plt.xticks(
                list(scipy.unique(symbols)),
                list(scipy.unique(symbols)))
            plt.yticks(list(scipy.unique(y_scale)), list(scipy.unique(y_scale)))
            plt.title('EXPANSION=2 AND DENSITY: 0.1-0.9:' + type.upper())
            plt.legend(loc='best', prop={"size": 10})
            plt.grid(True)
            pdf.savefig(dpi=900)
            plt.close()


if __name__ == '__main__':
    args = ['encoder', 'decoder']
    plot(args)