#! /usr/bin/env python
from _threading_local import local

import numpy as np
import pandas as pd
import scipy
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages as pdfpages

print("Using pandas {}".format(pd.__version__))

df = pd.read_csv('out_fulcrum.csv')
dffd = pd.read_csv('out_fulcrum_sparse.csv')
df_rlnc = pd.read_csv('out_rlnc.csv')
def plot(args):

    symbols = df['symbols']
    y_scale = [20, 50, 100, 200, 300, 1000, 2000, 3000]

    filename = "fulcrumsparse_kodofulcrum.pdf"
    with pdfpages(filename) as pdf:
        for type in args:
            expansion_df = df[df['type'] == type]

            if len(expansion_df) == 0:
                print("Skipping full no results")

            # throughput = df.groupby(['expansion', 'symbols', 'testcase'])['throughput'].mean()
            #
            # # throughput RLNC
            # throughput_rlnc = df_rlnc[df_rlnc['type'] == type]
            # throughput_rlnc = df_rlnc.groupby(['benchmark', 'symbols'])['throughput'].mean()
            # throughput_rlnc = throughput_rlnc.unstack(level=0)

            #throughput FULCRUM+DENSITY
            df_fd = dffd[dffd['type'] == type]

            throughput_exp1_den02 = df_fd[(df_fd['expansion'] == 1) & (df_fd['density'] == 0.2)]
            throughput_exp1_den02 = throughput_exp1_den02.groupby(['symbols', 'testcase'])['throughput'].mean()
            throughput_exp1_den02 = throughput_exp1_den02.unstack(level=1)

            throughput_exp1_den05 = df_fd[(df_fd['expansion'] == 1) & (df_fd['density'] == 0.5)]
            throughput_exp1_den05 = throughput_exp1_den05.groupby(['symbols', 'testcase'])['throughput'].mean()
            throughput_exp1_den05 = throughput_exp1_den05.unstack(level=1)

            throughput_exp4_den05 = df_fd[(df_fd['expansion'] == 4) & (df_fd['density'] == 0.5)]
            throughput_exp4_den05 = throughput_exp4_den05.groupby(['symbols', 'testcase'])['throughput'].mean()
            throughput_exp4_den05 = throughput_exp4_den05.unstack(level=1)

            # throughput_exp1_den02 = throughput_exp1_den02.unstack(level=1) #Khong can lenh nay vi testcase trong t/h nay chi co 1 InnerDecoder

            throughput_expansion1 = df[(df['expansion']==1) & (df['type'] == type)]
            throughput_expansion1 = throughput_expansion1.groupby(['symbols', 'testcase'])['throughput'].mean()
            throughput_expansion1 = throughput_expansion1.unstack(level = 1)

            throughput_expansion4 = df[(df['expansion']==4) & (df['type'] == type)]
            throughput_expansion4 = throughput_expansion4.groupby(['symbols', 'testcase'])['throughput'].mean()
            throughput_expansion4 = throughput_expansion4.unstack(level = 1)


            plt.figure()
                # plt.plot(throughput_rlnc.index, throughput_rlnc['Binary'], color='y', label='RLNC Binary')
                # plt.plot(throughput_rlnc.index, throughput_rlnc['Binary8'], color='y', linestyle='--', label='RLNC Binary8')

            plt.plot(throughput_exp1_den02.index, throughput_exp1_den02['FulcrumInner'],
                         label="FulcrumInner r=1 + Density=0.2", color='r')
            plt.plot(throughput_exp1_den02.index, throughput_exp1_den02['FulcrumOuter'],
                         label="FulcrumOuter r=1 + Density=0.2", color='b')

            plt.plot(throughput_exp1_den02.index, throughput_exp1_den02['FulcrumCombined'],
                         label="FulcrumCombined r=1 + Density=0.2", color='y')

            plt.plot(throughput_exp4_den05.index, throughput_exp4_den05['FulcrumInner'],
                         label="FulcrumInner r=4 + Density=0.5", color='g')

            plt.plot(throughput_exp4_den05.index, throughput_exp4_den05['FulcrumOuter'],
                         label="FulcrumOuter r=4 + Density=0.5", color='c')

            plt.plot(throughput_exp4_den05.index, throughput_exp4_den05['FulcrumCombined'],
                         label="FulcrumCombined r=4 + Density=0.5", color='gray')

            plt.plot(throughput_expansion1.index, throughput_expansion1['FulcrumInner'], label = 'FulcrumInner r=1',
                         color='r', linestyle='--')
            plt.plot(throughput_expansion1.index, throughput_expansion1['FulcrumOuter'], label='FulcrumOuter r=1',
                         color='b', linestyle='--')
            plt.plot(throughput_expansion1.index, throughput_expansion1['FulcrumCombined'], label='FulcrumCombined r=1',
                         color='y', linestyle='--')

            plt.plot(throughput_expansion4.index, throughput_expansion4['FulcrumInner'], label='FulcrumInner r=4',
                         color='g', linestyle='--')

            plt.plot(throughput_expansion4.index, throughput_expansion4['FulcrumOuter'], label='FulcrumOuter r=4',
                         color='c', linestyle='--')

            plt.plot(throughput_expansion4.index, throughput_expansion4['FulcrumCombined'], label='FulcrumCombined r=4',
                         color='gray', linestyle='--')

            plt.ylabel('Throughput [Mbps]')
            plt.xlabel('Generation size')
            plt.yscale('log', linthreshy = 10)
            plt.xscale('log', basex = 2)
            plt.xticks(
                    list(scipy.unique(symbols)),
                    list(scipy.unique(symbols)))
            plt.yticks(list(scipy.unique(y_scale)),list(scipy.unique(y_scale)))
            plt.title('FULCRUM AND SPARSE:' + type.upper())
                #
                # plt.legend(loc='center right', bbox_to_anchor= (1.3, 0.5))
                    # plt.yscale('symlog', linthreshy = 10)
                # lgd=plt.legend(loc='center left', prop={"size":10}, bbox_to_anchor=(1,0.5))
            plt.legend(loc='best', prop={"size": 9})
            plt.grid(True)
            pdf.savefig(dpi=300)
            plt.close()

    filename = "density comparison.pdf"
    with pdfpages(filename) as pdf:
        for type in args:
            df_fd = dffd[dffd['type'] == type]

            throughput_exp1_den02 = df_fd[(df_fd['expansion'] == 1) & (df_fd['density'] == 0.2)]
            throughput_exp1_den02 = throughput_exp1_den02.groupby(['symbols', 'testcase'])['throughput'].mean()
            throughput_exp1_den02 = throughput_exp1_den02.unstack(level=1)

            throughput_exp1_den05 = df_fd[(df_fd['expansion'] == 1) & (df_fd['density'] == 0.5)]
            throughput_exp1_den05 = throughput_exp1_den05.groupby(['symbols', 'testcase'])['throughput'].mean()
            throughput_exp1_den05 = throughput_exp1_den05.unstack(level=1)

            plt.figure()

            plt.plot(throughput_exp1_den05.index, throughput_exp1_den05['FulcrumInner'],
                     label="FulcrumInner r=1 + Density=0.5", color='r')
            plt.plot(throughput_exp1_den05.index, throughput_exp1_den05['FulcrumOuter'],
                     label="FulcrumOuter r=1 + Density=0.5", color='b')

            plt.plot(throughput_exp1_den05.index, throughput_exp1_den05['FulcrumCombined'],
                     label="FulcrumCombined r=1 + Density=0.5", color='y')


            plt.plot(throughput_exp1_den02.index, throughput_exp1_den02['FulcrumInner'],
                     label='FulcrumInner r=1 + Density=0.2', color='r', linestyle='--')
            plt.plot(throughput_exp1_den02.index, throughput_exp1_den02['FulcrumOuter'],
                     label='FulcrumOuter r=1 + Density=0.2', color='b', linestyle='--')
            plt.plot(throughput_exp1_den02.index, throughput_exp1_den02['FulcrumCombined'],
                     label='FulcrumCombined r=1 + Density=0.2', color='y', linestyle='--')

            plt.ylabel('Throughput [Mbps]')
            plt.xlabel('Generation size')
            plt.yscale('log', linthreshy=10)
            plt.xscale('log', basex=2)
            plt.xticks(
                    list(scipy.unique(symbols)),
                    list(scipy.unique(symbols)))
            plt.yticks(list(scipy.unique(y_scale)), list(scipy.unique(y_scale)))
            plt.title('FULCRUM SPARSE: DENSITY 0.2 AND 0.5 - ' + type.upper())
                #
                # plt.legend(loc='center right', bbox_to_anchor= (1.3, 0.5))
                # plt.yscale('symlog', linthreshy = 10)
                # lgd=plt.legend(loc='center left', prop={"size":10}, bbox_to_anchor=(1,0.5))
            plt.legend(loc='best', prop={"size": 9})
            plt.grid(True)
            pdf.savefig(dpi=300)
            plt.close()



if __name__ == '__main__':
    args = ['encoder', 'decoder']
    plot(args)