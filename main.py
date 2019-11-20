from binance.client import Client
from binance.websockets import BinanceSocketManager
import dateparser
from time import time
from twisted.internet import reactor

def process_message(msg):
    try:
        msg['timestamp'] = time()
        f = open("my_data.csv","a+")
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

def startDepthSocketManager(client):
    bm = BinanceSocketManager(client)
    depth_BNBBTC = bm.start_depth_socket('BNBBTC', process_message, depth=BinanceSocketManager.WEBSOCKET_DEPTH_5)
    depth = bm.start_depth_socket('LTCBTC', process_message, depth=BinanceSocketManager.WEBSOCKET_DEPTH_5)
    bm.start()
    return bm, depth
    
try:
    client = startClient()
    socket_manager, depth = startDepthSocketManager(client)
except:
    display('ASDASD')
