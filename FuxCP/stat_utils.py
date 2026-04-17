
#%%

# ================ FIGURE EXTRACTION ================
def extract_from_line(lines, lineIdx, sep):
    line = lines[lineIdx].strip(' {()\n;')
    return line.split(sep)[-1].strip()

def extract_from_figure_test():
    string = ""
    with open("c++/src/figureTests.cpp", "r") as file:
        lines = file.readlines()
        for i in range(len(lines)) :
            j = 3
            line = lines[i].strip()
            if line.split(' ')[0] == 'spList': # We are in a figure !
                if("FIFTH_SPECIES" in line) : pass # special case
                string += extract_from_line(lines, i-2, '_') + ";"  # figName
                string += extract_from_line(lines, i, '=') + ";"    # spList
                string += extract_from_line(lines, i+1, '=') + ";"  # cantusFirmus
                while (lines[i+j].strip().split(' ')[0] != 'v_type'): j+=1
                string += extract_from_line(lines, i+j, '=') + "\n" # vType
            i+=j+2
    

    with open("c++/log/figures.csv", "a") as file:
        file.write("figName;spList;cantusFirmus;vType\n")
        file.write(string)


#%%

# ================ FIGURE DATA ================
import pandas as pd

def normalize(cf):
    return cf.replace(" ", "")

def generate_figure_data():
    figures = {}
    
    df = pd.read_csv("c++/log/figures.csv",sep=";")
    df['cantusFirmus'] = df['cantusFirmus'].apply(normalize)

    for cf in df['cantusFirmus']:
        if cf in figures:
            figures[cf] += 1
        else :
            figures[cf] = 1

    return figures

figs = generate_figure_data()

#%%


# ================ ANALYZE STATS ================
import os
import random
import pandas as pd
from statistics import mean
from math import sqrt
import matplotlib.pyplot as plt

def transfer_data():
    
    #df = pd.read_csv("c++/log/default_stats.csv",sep=",")
    folder_path = "c++/log/stats_S11/"
    
    with open("c++/log/default_stats.csv", 'a') as outFile:
        for filename in os.listdir(folder_path):
            with open(folder_path + filename) as readFile:
                lines = readFile.readlines()[1:] # get rid of first line
                for line in lines:
                    timestamp = line.split(',')[0].replace(' ', '_')
                    restOfLine = line[line.find(',')+1:-2] + "\n" # line without timestamp and last coma
                    restOfLine = restOfLine.replace(" ","").replace("{","").replace("}","")
                    
                    outFile.write(timestamp + ',' + filename + ',' + restOfLine)
                    
#transfer_data()


def costs_str_to_array(costs_str):
    costs_list = costs_str.split(',')
    for i in range(len(costs_list)):
        costs_list[i] = int(costs_list[i])
    return costs_list

def mean_without_big(L):
    sum = 0.0
    for val in L:
        if val <= 300 : sum += val
    return sum / len(L)

def mean_of_sqrt(L):
    sum = 0.0
    for val in L:
        sum += sqrt(val)
    return sum / len(L)

def take_first(L):
    return L[0]


df = pd.read_csv("c++/log/default_stats.csv",sep=",")
df["costs"] = df["costs"].apply(costs_str_to_array)
df["solution"] = df["solution"].apply(costs_str_to_array)
df["first_cost"] = df["costs"].apply(take_first)
df["costs_mean"] = df["costs"].apply(mean)
df["costs_mean_without_big"] = df["costs"].apply(mean_without_big)
df["costs_sqrt_mean"] = df["costs"].apply(mean_of_sqrt)

def plot_costs_evolution():
    run_names = df["filename"].unique()

    for i in range(len(run_names)):
        #idx = random.randint(0,len(run_names)-1)
        name = run_names[i]
        
        sub_df = df[df["filename"] == name]
        
        plt.plot(sub_df["iteration"], sub_df["costs_mean"], linewidth=3.0, label="Mean")
        plt.plot(sub_df["iteration"], sub_df["costs_mean_without_big"], label="Mean \\ big")
        plt.plot(sub_df["iteration"], sub_df["costs_sqrt_mean"], label="Sqrt mean")
        plt.plot(sub_df["iteration"], sub_df["first_cost"], label="First cost")
        plt.title(i)
        plt.legend()
        plt.show()


# ===== Costs analysis =====
costs_values = {}
big_costs_idx_set = set()

for L in df["costs"]:
    for i in range(len(L)):
        cost = L[i]
        if cost in costs_values:
            costs_values[cost] += 1
        else :
            costs_values[cost] = 1
            
        if cost > 300:
            big_costs_idx_set.add(i)

costs_list = list(costs_values.keys())
costs_list.sort()

def plot_costs_repartition():
    plt.plot(costs_list, [min(costs_values[cost], 1000) for cost in costs_list], linewidth='1.0', marker='.')
    plt.show()



#analyze_stats()


#%%

