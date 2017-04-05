#! /usr/bin/env python
from _threading_local import local

import numpy as np
import pandas as pd
import scipy
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages as pdfpages

print("Using pandas {}".format(pd.__version__))


dffd = pd.read_csv('out_fulcrum_sparse.csv')

def plot(args):

    #df = df.set_index(['symbols', 'testcase', 'type'])
    # df_average = df.groupby(['expansion', 'symbols', 'type'])['throughput'].mean()
    filename = "expansion_density.pdf"
    with pdfpages(filename) as pdf:
        for type in args:

            df_fd = dffd[dffd['type'] == type]
            df_fd = df_fd[(df_fd['symbols'] == 16) & (df_fd['testcase'] == 'FulcrumInner')]
            # print df_fd
            expansion_density = df_fd.groupby(['expansion', 'density'])['throughput'].mean()
            expansion_density = expansion_density.unstack(level=0)
            print expansion_density

            plt.figure()
            plt.plot(expansion_density.index, expansion_density[1], label='r=1')
            plt.plot(expansion_density.index, expansion_density[2], label='r=2')
            plt.plot(expansion_density.index, expansion_density[3], label='r=3')
            plt.plot(expansion_density.index, expansion_density[4], label='r=4')

            plt.ylabel('Throughput [Mbps]')
            plt.xlabel('Density')

            plt.title('EXPANSION vs DENSITY:' + type.upper())

            plt.legend(loc='right upper', prop={"size": 10})
            plt.grid(True)
            # plt.xticks(list(scipy.unique(expansion_density.index)), list(scipy.unique(expansion_density.index)))
            plt.xlim([0.1, 0.9]) #limited the axes value
            pdf.savefig(dpi=300)
            plt.close()


if __name__ == '__main__':
    args = ['encoder', 'decoder']
    plot(args)