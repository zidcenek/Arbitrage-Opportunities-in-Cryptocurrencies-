from binance.client import Client
from binance.websockets import BinanceSocketManager
import dateparser
from time import time
from twisted.internet import reactor
from datetime import datetime
from datetime import date
import copy
import pandas as pd
import sys
from pathlib import Path

class BinanceWebsocket():
    def __init__(self, directory):
        self.DEPTH = BinanceSocketManager.WEBSOCKET_DEPTH_5
        self.CURRENT_DATE = date.today()
        self.DIRECTORY = directory
        self.CURRENCIES = []
        symbols = pd.read_csv("./relevant_symbols", header=None)
        for symbol in symbols[0]:
            self.CURRENCIES.append({'currency': symbol, 'file': None})
    
    
    def calculate_files(self):
        '''
            Calculates a path and creates a file for every currency pair in relevant_symbols file.
        '''
        tmp_list = self.CURRENCIES
        for i in range(len(self.CURRENCIES)):
            self.CURRENCIES[i]['file'] = open(self.DIRECTORY + self.CURRENCIES[i]['currency'] + "-" + str(self.CURRENT_DATE),"a+")

            
    def make_lambda(self, curr):
        '''
            Makes a labda function reffering to prcess_message function
        '''
        return lambda msg: self.process_message(msg, curr)
    
    
    def update_current_date(self):
        '''
            Updates date every day.
        '''
        self.CURRENT_DATE = date.today()


    def process_message(self, msg, currency=None):
        '''
            Takes care of a timestamp addition and data collection.
        '''
        if currency is None:
            return None
        try:
            msg['timestamp'] = time()
            if not self.CURRENT_DATE == date.today():
                self.update_current_date()
                self.calculate_files()
            tmp = ""
            for item in msg.values():
                tmp += str(item) + ';'
            tmp += '/n'
            currency['file'].write(tmp)
        except Exception as e:
            print(e)
        return None


    def start_client(self):
        '''
            Starts a client communicating with the Binance exchange
        '''
        with open('apikey.txt', 'r') as apikey:
            api_key = apikey.read()
        with open('privatekey.txt', 'r') as privatekey:
            api_secret = privatekey.read()
        return Client(api_key, api_secret)


    def start_depth_socket_manager(self, client):
        '''
            Starts a websocket for every currency pair
        '''        
        bm = BinanceSocketManager(client)
        queries = []
        for currency in self.CURRENCIES:
            queries.append(bm.start_depth_socket(currency['currency'], self.make_lambda(currency), depth=self.DEPTH))
        bm.start()
        return bm, queries

def initialize():
    if len(sys.argv) != 2:
        print("Please enter an output directory path as a first argument")
        return 0;
    output_directory = sys.argv[1]
    if output_directory[-1] != '/':
        output_directory += '/'
    Path(output_directory).mkdir(parents=True, exist_ok=True)

    try:
        binance = BinanceWebsocket(output_directory) 
        binance.calculate_files()
        client = binance.start_client()
        socket_manager, queries = binance.start_depth_socket_manager(client)
        print('init end')
    except Exception as e:
        print(e)
  
initialize()