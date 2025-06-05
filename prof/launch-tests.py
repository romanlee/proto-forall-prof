import os
import sys
import json

# setup the runs and launch
with open('input-template.json', 'r') as file:
    input = json.load(file)

# set the desired hyper params
nx_vals = [4,16,32,64]
N_vals = [5,10,20,40,80,160,320,640]

os.makedirs('runs/', exist_ok=True)
os.chdir('runs/')
root_dir = os.getcwd()

for nx in nx_vals:
    input['nx'] = nx

    for N in N_vals:
        for type in ['heavy', 'light']:
            sim_dir = 'nx_{}/N_{}/{}/'.format(nx, N, type)
            if os.path.isdir(sim_dir):
                print('Directory runs/{} exists... Skipping'.format(sim_dir))

            else:
                os.makedirs(sim_dir)
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
                command = 'nsys profile -o ./report-nx_{}-N_{}-{} ../../../../forall_constoprim'.format(nx, N, type)
                print("current dir: " + os.getcwd())
                print(command)
                os.system(command)
                print('')

                os.chdir(root_dir)
