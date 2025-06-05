import os
import json
import slurp_prtimerrep
import pandas as pd
from matplotlib import pyplot as plt
import numpy as np


def get_leaf_dirs(root_path):
    """
    Returns a list of directories that contain no subdirectories
    (they may contain files, but no folders)
    """
    leaf_dirs = []
    
    for dirpath, dirnames, filenames in os.walk(root_path):
        if not dirnames:  # No subdirectories
            leaf_dirs.append(dirpath+'/')
    
    return leaf_dirs


def get_results(run_dir):
    """
    Gather profiling results into pandas dataframe
    """
    leaf_dirs = get_leaf_dirs(run_dir)

    results = []

    # add data
    for path in leaf_dirs:
        # read hyper params from input deck
        with open(path+'input.json', 'r') as f:
            params = json.load(f)

        # read forallzone timings
        with open(path+'TIMINGS.txt', 'r') as f:
            timings = f.read()
        timing_data = slurp_prtimerrep.parse_timing_report(timings)
        forall_zone_timer = slurp_prtimerrep.find_timer_by_name(timing_data, 'forall_zone')

        # gather results
        results.append({'nx'   : params['nx'], 
                        'N_ext': params['N_ext'],
                        'N_int': params['N_int'], 
                        'time' : forall_zone_timer['time']})

    return pd.DataFrame(results)


results = get_results('../runs')
nx_vals = results['nx'].unique()

plt.figure()

for nx in nx_vals:
    # plot_nx(results, nx)

    # def plot_nx(results, nx):
    #     """
    #     Plot light/heavy time vs N for given value of nx
    #     """
    # savefig = 'nx_{}.png'.format(nx)
    # print("Plotting {}".format(savefig)) 

    # pandas dataframes are great
    heavy = results[ (results['nx'] == nx) & (results['N_ext']==1) ].sort_values('N_int')
    light = results[ (results['nx'] == nx) & (results['N_int']==1) ].sort_values('N_ext')

    # get heavy times
    htime_vals = heavy['time'].to_numpy()

    # get light times noramlized to heavy
    ltime_vals = light['time'].to_numpy()
    ltime_vals = np.divide(ltime_vals, htime_vals)

    N_vals = light['N_ext'].to_numpy()

    #  plot
    plt.scatter(N_vals, ltime_vals, label='nx = {}'.format(nx))
    plt.plot(N_vals, ltime_vals, linestyle='--')

plt.legend()
plt.xlabel('iterations N')
plt.ylabel('light/heavy time')
plt.savefig('nx_all.png')
