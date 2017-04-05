import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages as pdfpages
import sys
import scipy
import os

from numpy.core.numeric import full
from pandas.lib import fast_unique_multiple_list

sys.path.insert(0, os.path.join(
    os.path.dirname(os.path.realpath(__file__)), ".."))
import plot_helper

print("Using pandas {}".format(pd.__version__))
df = pd.read_csv('out_fulcrumsparse_recoding.csv')

full_df = df
if len(full_df) == 0:
    print("Skipping full no results")
else:
    print("not empty dataframe")
# full_df = full_df.set_index(['benchmark', 'symbols', 'testcase'])

# print full_df.values
# a=np.array([[1,2],[3,4]])
# print np.mean(a) #return the average of the array....

full_df['mean'] = full_df['used'].apply(scipy.mean) - full_df['symbols']

# print full_df['used'].apply(scipy.mean) - full_df['symbols']

full_df['std'] = full_df['used'].apply(scipy.std)
full_df['field'] = full_df['benchmark'].apply(plot_helper.get_field)
full_df['algorithm'] = full_df['testcase'].apply(plot_helper.get_algorithm)
full_df['slavename'] = "local"

#isin(): loc ra cac gia tri co trong danh sach
dense = full_df[full_df['testcase'].isin(["FulcrumSparseInner"])] #"dense": ["FullRLNC", "BackwardFullRLNC", "FullDelayedRLNC"],
sparse = df[df['testcase'].isin(plot_helper.codes['sparse'])]         # "sparse": ["SparseFullRLNC"],

decodingprob = full_df[(full_df['expansion'] == 2) & (full_df['erasure1'] == 0.1) & (full_df['symbols'] == 128)& (full_df['density'] == 0.2)]
decodingprob = decodingprob.groupby(['erasure2', 'testcase'])['mean'].mean()
decodingprob = decodingprob.unstack('testcase')

print decodingprob
filename = "Additionalpacket_transmitted.pdf"
with pdfpages(filename) as pdf:
    plt.figure()
    plt.plot(decodingprob.index, decodingprob['FulcrumSparseInner'], label="FulcrumSparseInner")
    plt.plot(decodingprob.index, decodingprob['FulcrumSparseOuter'], label="FulcrumSparseOuter")
    plt.plot(decodingprob.index, decodingprob['FulcrumSparseCombined'], label="FulcrumSparseCombined")
    plt.xlabel("Generation size")
    plt.ylabel("Additional packets")
    plt.title('ADDITIONAL PACKETS exp=2, density=0.2 and first link erasure=20%')
    # plt.xscale('log', basex=2)
    plt.xticks(list(scipy.unique(decodingprob.index)), list(scipy.unique(decodingprob.index)))
    plt.grid(True)
    plt.legend(loc='best')
    pdf.savefig(dpi=900)
    plt.close()

# dense = dense.set_index('symbols')
# dense = dense.unstack('testcase')

# dense = dense[(dense['symbols'] == 16)]



# plt.figure()
# for (benchmark, symbols), group in dense:
#     p = group.pivot_table('mean', rows='symbols', cols=['field',
#                                                         'density']).plot()


# p = dense.pivot_table('mean', rows='symbols', cols=['field', 'density'])
#
# y = full_df.groupby(['benchmark', 'symbols', 'testcase'])['mean'].mean()
# print p
# plt.figure()
# plot(title=title)
# plt.ylabel('[micor seconds]')
# plt.show()



# print time.index.levels[0]
# for benchmark in time.index.levels[0]:
#     y = time[benchmark].unstack('testcase')
#     yerr = error[benchmark].unstack('testcase')
#     title = "field={}".format(benchmark)
#     plt.figure() #create a new figure
#     y.plot(title=title, yerr=yerr)
#     plt.ylabel('[micro seconds]')
#     # pdf.savefig()
# plt.show()


# # Run the results for sparse
# sparse_df = df[df['density'].notnull()]
#
# if len(sparse_df) == 0:
#     print("Skipping full no results")
#
# sparse_df = sparse_df.set_index(['benchmark', 'symbols', 'density', 'testcase'])
#
# # sparse_df.to_csv(sys.stdout)
#
# time = sparse_df['time'].apply(np.mean)
# # time.to_csv(sys.stdout)
# error = sparse_df['time'].apply(np.std)
# print("Deviation:".format(time))
# # error.to_csv(sys.stdout)
# # print error
# print time.index.levels[0]
# print "----------------------------------"
# print time.index.levels[1]
# y = time['binary'][16].unstack('testcase')
# print y
# yerr = error['binary'][16].unstack('testcase')
#
# print yerr
# title = "filed={}, symbols={}".format('binary',16)
# # plt.figure()
# y.plot(title=title)
# plt.ylabel('[micor seconds]')
# plt.show()
