import os
import json

# set the desired hyper params
nx_vals = [16]
N_vals = [5,10,20]

# setup the runs and launch
with open('input-template.json', 'r') as file:
    input = json.load(file)

for nx in nx_vals:
    input['nx'] = nx
    for N in N_vals:
        for type in ['heavy', 'light']:
            # create directory
            path = 'nx_{}/N_{}/{}/'.format(nx, N, type)
            os.makedirs(path, exist_ok=True)

            # write input deck to directory
            if type=='heavy':
                input['N_ext'] = 1
                input['N_int'] = N
            elif type=='light':
                input['N_ext'] = N
                input['N_int'] = 1

            with open(path + 'input.json', 'w') as file:
                json.dump(input, file, indent=2)

            # launch simulation