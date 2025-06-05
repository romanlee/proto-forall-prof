import os
import sys
import json

root_dir = os.getcwd()

# set the desired hyper params
nx_vals = [16]
N_vals = [10]

# setup the runs and launch
with open('input-template.json', 'r') as file:
    input = json.load(file)

for nx in nx_vals:
    input['nx'] = nx

    path = 'nx_{}'.format(nx)
    if os.path.exists('nx_{}'.format(nx)):
        print("Directory {} exists. Exiting...".format(path))
        sys.exit(1)
    else:
        for N in N_vals:
            for type in ['heavy', 'light']:
                # create directory
                sim_dir = 'nx_{}/N_{}/{}/'.format(nx, N, type)
                os.makedirs(sim_dir, exist_ok=True)

                os.chdir(sim_dir)

                # write input deck to directory
                if type=='heavy':
                    input['N_ext'] = 1
                    input['N_int'] = N
                elif type=='light':
                    input['N_ext'] = N
                    input['N_int'] = 1

                with open('input.json', 'w') as file:
                    json.dump(input, file, indent=2)

                # launch simulation
                command = 'nsys profile -o ./report-nx_{}-N_{}-{} ../../../forall_constoprim'.format(nx, N, type)
                print("current dir: " + os.getcwd())
                print("    " + command)
                os.system(command)

                os.chdir(root_dir)
