#! /usr/bin/env python
from _threading_local import local

import numpy as np
import pandas as pd
import scipy
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages as pdfpages

print("Using pandas {}".format(pd.__version__))

df = pd.read_csv('output.csv')
df_my = pd.read_csv('output_mybenchmark.csv')
def plot(args):

    #df = df.set_index(['symbols', 'testcase', 'type'])
    # df_average = df.groupby(['expansion', 'symbols', 'type'])['throughput'].mean()

    for type in args:
        expansion_df = df[df['type'] == type]
        symbols=df['symbols']
        y_scale = [1, 5, 10, 20, 50, 100, 200, 300, 1000, 2000]
        if len(expansion_df) == 0:
            print("Skipping full no results")

        throughput = df.groupby(['expansion', 'symbols', 'testcase'])['throughput'].mean()
        throughput_my = df.groupby(['expansion', 'symbols', 'testcase'])['throughput'].mean()
        error = df.groupby(['expansion', 'symbols', 'testcase'])['throughput'].apply(np.std)
        # throughput = throughput.set_index(['expansion', 'symbols', 'testcase'])

        throughput_expansion1 = df[(df['expansion']==1) & (df['type'] == type)]
        throughput_expansion1 = throughput_expansion1.groupby(['symbols', 'testcase'])['throughput'].mean()
        throughput_expansion1 = throughput_expansion1.unstack(level = 1)

        m_throughput_expansion1 = df_my[(df['expansion'] == 1) & (df['type'] == type)]
        m_throughput_expansion1 = m_throughput_expansion1.groupby(['symbols', 'testcase'])['throughput'].mean()
        m_throughput_expansion1 = m_throughput_expansion1.unstack(level=1)

        filename = type + "_fulcrum_kodo_mybenchmark_throughput.pdf"
        with pdfpages(filename) as pdf:
            plt.figure()

            plt.plot(throughput_expansion1['FulcrumInner'], label = 'Inner r=1', color='b')
            plt.plot(throughput_expansion1['FulcrumOuter'], label = 'Outer r=1', color='r')
            plt.plot(throughput_expansion1['FulcrumCombined'], label = 'Combined r=1', color='y')

            plt.plot(m_throughput_expansion1['FulcrumInner'], label='My Inner r=1', color='b', linestyle='--')
            plt.plot(m_throughput_expansion1['FulcrumOuter'], label='My Outer r=1', color='r', linestyle='--')
            plt.plot(m_throughput_expansion1['FulcrumCombined'], label='My Combined r=1', color='y', linestyle='--')

            plt.ylabel('Throughput [Mbps]')
            plt.xlabel('Generation size')
            plt.yscale('log', linthreshy = 10)
            plt.xscale('log', basex = 2)
            plt.xticks(
                list(scipy.unique(symbols)),
                list(scipy.unique(symbols)))
            plt.yticks(list(scipy.unique(y_scale)),list(scipy.unique(y_scale)))
            plt.title('FULCRUM AND RLNC:' + type.upper())
            #
            # plt.legend(loc='center right', bbox_to_anchor= (1.3, 0.5))
                # plt.yscale('symlog', linthreshy = 10)
            # lgd=plt.legend(loc='center left', prop={"size":10}, bbox_to_anchor=(1,0.5))
            plt.legend(loc='best', prop={"size": 10})
            plt.grid(True)
            pdf.savefig(dpi =300)
            plt.close()
        # filename = type + "_throughput.pdf"
        # with pdfpages(filename) as pdf:
        #     for expansion in throughput.index.levels[0]:
        #         y = throughput[expansion].unstack('testcase')
        #         yerr = error[expansion].unstack('testcase')
        #         title = "expansion={}".format(expansion)
        #         plt.figure()
        #
        #         #y.plot(title=title, grid = True)
        #         plt.plot(y['FulcrumInner'], linewidth=2)
        #         plt.plot(y['FulcrumOuter'], linewidth=2)
        #         plt.plot(y['FulcrumCombined'], linewidth=2)
        #         # plt.axis([16, 1024, 10, 300])  # plt.axis([xmin, xmax, ymin, ymax])
        #         plt.plot(throughput_my['Binary'])
        #         plt.plot(throughput_my['Binary8'])
        #         plt.ylabel('Throughput [Mbps]')
        #         plt.xlabel('Generation size')
        #         plt.yscale('log', linthreshy = 10)
        #         plt.xscale('log', basex = 2)
        #         plt.xticks(
        #             list(scipy.unique(symbols)),
        #             list(scipy.unique(symbols)))
        #         plt.yticks(list(scipy.unique(y_scale)),list(scipy.unique(y_scale)))
        #         plt.title(title)
        #         plt.legend(loc = 'best', prop={"size":10}) #change the size of legend
        #         plt.grid(True)
        #         # plt.yscale('symlog', linthreshy = 10)
        #         pdf.savefig()
        #         plt.close()

if __name__ == '__main__':
    args = ['encoder', 'decoder']
    plot(args)