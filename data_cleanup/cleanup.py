import pandas as pd
from os.path import isfile, join, isdir, getsize
from os import listdir
import numpy as np
from pathlib import Path
from time import time
import sys

def fetch_directories(path: str) -> [str]:
    '''
    Fetches all directories in the given path 
    '''
    directories = []
    try:
        directories = [join(path, f) for f in listdir(path) if isdir(join(path, f))] 
    except:
        print("Trouble opening a directory")
        return directories
    return directories

def fetch_files(path: str) -> [str]:    
    '''
    Fetches all files in the given path 
    '''
    files = []
    try:
        files = [f for f in listdir(path) if isfile(join(path, f))] 
    except:
        print("Trouble opening a directory")
        return files
    return files

def is_relevant_timestamp(x: any) -> bool:
    '''
    Checks if the value x is relevant timestamp (dates between year 2018 and today)
    '''
    try:
        fl = float(x)
        if not (1500000000 < fl < max_time):
            return False
    except ValueError:
        return False
    return True

def clean_csv(filename: str) -> pd.DataFrame:
    '''
        Cleans a specific file.
    '''
    print('Cleaning', filename)
    if getsize(filename) == 0:
        return None
    cols = pd.read_csv(filename, nrows=1, delimiter=';').columns
    df = pd.read_csv(filename, usecols=cols, delimiter=';', skip_blank_lines=True, error_bad_lines=False, 
                         warn_bad_lines=False, dtype=str)
    
    if len(df.columns) == 5:
        df.columns = ['id', 'demand', 'supply', 'timestamp', 'none']
        df = df.drop(['none'], axis=1)
    elif len(df.columns) == 4:
        df.columns = ['id', 'demand', 'supply', 'timestamp']
    else:
        return None
    df = df[df.id.apply(lambda x: x.isnumeric())]
    df.id = df.id.astype(int)
    df = df[df.timestamp.apply(lambda x: is_relevant_timestamp(x))]
    df = df.sort_values(['id', 'timestamp'])
    df = df.dropna()
    df = df.drop_duplicates('id')
    return df

def cleanup(path: str, output_path: str, children_directories: bool):
    '''
    Goes through all the files in a specific directory
    '''

    if children_directories:
        for directory in fetch_directories(path):
            for f in fetch_files(directory):
                if len(f) < 10:
                    continue
                print(directory + '/' + f)
                df = clean_csv(directory + '/' + f)
                if df is not None:
                    Path(output_path + directory[len(path):]).mkdir(parents=True, exist_ok=True)
                    df.to_csv(output_path + directory[len(path):] + '/' + f, sep=';', header=False, index=False)
                df = None
    else:
        directory = path
        for f in fetch_files(directory):
            if len(f) < 10:
                print("not supported filename")
                continue
            print(directory + '/' + f)
            df = clean_csv(directory + '/' + f)
            if df is not None:
                Path(output_path + directory).mkdir(parents=True, exist_ok=True)
                df.to_csv(output_path + directory + '/' + f, sep=';', header=False, index=False)
            df = None
			
            
def initialize():
    if not (2 < len(sys.argv) < 5):
        print("Please enter an input and an output directory path as a first and second argument")
        return 0;
    children_directories = False
    if len(sys.argv) == 4 and sys.argv[3] == '-r':
        children_directories = True
    path = sys.argv[1]
    output_path = sys.argv[2]
    cleanup(path, output_path, children_directories=children_directories)

max_time = time()
initialize()