from binance.client import Client
from binance.websockets import BinanceSocketManager
import dateparser
from time import time
from twisted.internet import reactor
import pandas as pd
from datetime import datetime
import copy

DEPTH = BinanceSocketManager.WEBSOCKET_DEPTH_5

def process_message(msg, currency='no currency'):
    try:
        msg['timestamp'] = time()
        date = datetime.now()
        fname = currency + "_" + str(date.year) + "_" + str(date.month) + "_" + str(date.day)
        f = open(fname,"a+")
        for item in msg.values():
            f.write(str(item) + ";")
        f.write("\n")
        f.close()
    except Exception as e:
        print(e)
    return None
    
def startClient():
    api_key = 'aKNYXtMDVEsVyJCMNwcLGpAG9KVK9d5V568QCRADjDn5kumLeN9joKhnA4zIpBPd'
    api_secret = 'ziY288jGcFEdOv81RoWrAbAUf08LKwxpJ2EmKt1M2pHlhcB9UCaciGXHeIcZVFCZ'
    return Client(api_key, api_secret)

def startDepthSocketManager(client, currencies):
    bm = BinanceSocketManager(client)
    queries = []
#     for i in range(len(currencies)):
#         print("curr", currencies[i])
#         queries.append(bm.start_depth_socket(str(currencies[i]), lambda msg: process_message(msg, str(currencies[i])), depth=DEPTH))
    queries.append(bm.start_depth_socket(currencies[0], lambda msg: process_message(msg, currencies[0]), depth=DEPTH))
    queries.append(bm.start_depth_socket(currencies[1], lambda msg: process_message(msg, currencies[1]), depth=DEPTH))
    bm.start()
    return bm, queries
    
try:
    currencies = ['LTCBTC', 'BNBBTC']
    client = startClient()
    socket_manager, queries = startDepthSocketManager(client, currencies)
except Exception as e:
    display(e)
