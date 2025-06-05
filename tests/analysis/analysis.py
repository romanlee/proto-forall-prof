import os
import json
import slurp_prtimerrep
import pandas as pd

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

# get list of all test dirs
leaf_dirs = get_leaf_dirs('../tests')

# df = pd.DataFrame(columns=['nx', 'N_ext', 'N_int', 'time'])

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

    # print(path)
    # print('')

    # print(params['N_int'])
    # print('')

    # print(f"Found {forall_zone_timer['name']}:")
    # print(f"  Time: {forall_zone_timer['time']:.6f}s")
    # print(f"  Percentage: {forall_zone_timer['percentage']:.1f}%")
    # print(f"  Calls: {forall_zone_timer['count']}")
    # print(f"  Children: {len(forall_zone_timer['children'])}")
    # print('')





    # # Add rows one by one
    # df.loc[0] = ['Alice', 25, 'NYC']
    # df.loc[1] = ['Bob', 30, 'LA']

    # Or append (less efficient for many rows)
    # new_row = pd.DataFrame({'name': ['Charlie'], 'age': [35], 'city': ['Chicago']})
    # new_row = pd.DataFrame({'nx': [params['nx']], 
    #                         'N_ext': [params['N_ext']], 
    #                         'N_int': [params['N_int']], 
    #                         'time':[forall_zone_timer['time']]})
    # df = pd.concat([df, new_row], ignore_index=True)

    results.append({'nx': params['nx'], 
               'N_ext': params['N_ext'],
               'N_int': params['N_int'], 
               'time':forall_zone_timer['time']})

results = pd.DataFrame(results)
print(results)