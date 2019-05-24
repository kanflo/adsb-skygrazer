#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2019 Johan Kanflo (github.com/kanflo)
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

"""
    Subscribe to the proximity messages from proxclient.py, download images and
    print flight information.

    The ADSB message received have the following format:

    {
        "loggedDate": "2019-05-01 21:24:57.768000"
        "time": 1556738697
        "icao24": "4CA863"
        "altitude": 9450
        "lat": 56.13506
        "lon": 12.70419
        "distance": 65.02801099999999
        "callsign": "SAS2881"
        "operator": "Scandinavian Airlines System"
        "speed": 317
        "vspeed": -1856
        "heading": 166
        "bearing": 322
        "type": "Bombardier CRJ 900 LR NG"
        "registration": "EI-FPI"
        "image": "https://www.netairspace.com/photos/EI-FPI/Scandinavian_Airlines_SAS/Bombardier_CRJ-900LR/HAM_EDDH_Hamburg_Fuhlsbuettel/photo_159487/medium.jpg?uq=350201"}
        "route": {
            "origin": {
                "icao": "ENVA"
                "name": "Trondheim Airport Varnes"
                "city": "Trondheim"
                "country": "Norway"
                "iata": "TRD"
                "lat": "63.457802"
                "lon": "10.924000"
                "alt": "56"
            }
            "destination": {
                "icao": "EKCH"
                "name": "Copenhagen Kastrup Airport"
                "city": "Copenhagen"
                "country": "Denmark"
                "iata": "CPH"
                "lat": "55.617901"
                "lon": "12.656000"
                "alt": "17"
            }
        }
    }
"""

import sys
try:
    import paho.mqtt.client as mqtt
except ImportError:
    print("sudo -H pip3 install paho-mqtt")
    sys.exit(1)
try:
    import requests
except ImportError:
    print("sudo -H pip3 install requests")
    sys.exit(1)
try:
    import pygame
except ImportError:
    print("PyGame is not installed.")
    print("It can be somewhat tricky to install as pip3 install is far from sufficient.")
    print("If on a virgin Raspberry Pi, you will probably need the following:")
    print(" sudo apt update")
    print(" sudo apt install -y python3-setuptools")
    print(" sudo apt install -y python3-pip")
    print(" sudo apt install -y python3-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev libsdl1.2-dev libsmpeg-dev python-numpy subversion libportmidi-dev ffmpeg libswscale-dev libavformat-dev libavcodec-dev")
    print("And then:")
    print(" sudo -H pip3 install pygame")
    sys.exit(1)
try:
    import mqtt_wrapper
except ImportError:
    print('sudo -H pip3 install "git+https://github.com/EmaroLab/mqtt_wrapper.git"')
    print("Yes, you need the git version. For now.")
    sys.exit(1)
import time
import logging
from logging.handlers import RotatingFileHandler
import argparse
import configparser
import json
from collections import namedtuple
import datetime
import random
import io
import ssl
import urllib.request
from urllib.request import build_opener, HTTPCookieProcessor, Request
from urllib.parse import urlparse
import hashlib
import os.path
import glob


# Last received ADSB message
adsb_message = None

#os.putenv('SDL_VIDEODRIVER', 'fbcon')
#os.environ["SDL_VIDEODRIVER"] = "fbcon"

class mybridge(mqtt_wrapper.bridge):
    def msg_process(self, message):
        global adsb_message
        if 'json' in message.topic:
            adsb_message = json.loads(message.payload.decode("utf-8"), object_hook=lambda d: namedtuple('X', d.keys())(*d.values()))

def headless_init():
#        import pygame.display
        disp_no = os.getenv("DISPLAY")
        if disp_no:
            print("I'm running under X display = %s" % disp_no)
        # Check which frame buffer drivers are available
        # Start with fbcon since directfb hangs with composite output
        drivers = ['fbcon', 'directfb', 'svgalib']
        found = False
        for driver in drivers:
            # Make sure that SDL_VIDEODRIVER is set
            if not os.getenv('SDL_VIDEODRIVER'):
                os.putenv('SDL_VIDEODRIVER', driver)
            try:
                pygame.init()
            except pygame.error as e:
                print('Driver: %s failed.' % driver)
                print(e)
                continue
            found = True
            break
        if not found:
            raise Exception('No suitable video driver found!')


def pygame_init():
    fullscreen = True
    screen_width = None
    screen_height = None

    if "UI" in config:
        if "screen_width" in config["UI"]:
            screen_width = int(config["UI"]["screen_width"])
        if "screen_height" in config["UI"]:
            screen_height = int(config["UI"]["screen_height"])
        if "fullscreen" in config["UI"]:
            fullscreen = config["UI"]["fullscreen"] == "True"

#    headless_init()
#    os.putenv('SDL_VIDEODRIVER', 'fbcon')
#    pygame.display.init()
#    pygame.font.init()
    pygame.init()
#    os.putenv('SDL_VIDEODRIVER', 'fbcon')
#    pygame.display.init()
#    pygame.display.set_mode((1, 1))

    info = pygame.display.Info()
    if not screen_width:
        screen_width = info.current_w
    if not screen_height:
        screen_height = info.current_h

    if fullscreen:
        screen = pygame.display.set_mode((screen_width, screen_height), pygame.FULLSCREEN)
        pygame.mouse.set_visible(False)
    else:
        screen = pygame.display.set_mode((screen_width, screen_height))

    #pygame.mouse.set_cursor((8,8),(0,0),(0,0,0,0,0,0,0,0),(0,0,0,0,0,0,0,0))

    return screen


def blit_outlined_text(screen, font, str, x, y, text_color, outline_color, adjust = "left"):
    blit_text(screen, font, str, x + 1, y + 1, outline_color, adjust)
    blit_text(screen, font, str, x - 1, y + 1, outline_color, adjust)
    blit_text(screen, font, str, x + 1, y - 1, outline_color, adjust)
    blit_text(screen, font, str, x - 1, y - 1, outline_color, adjust)
    blit_text(screen, font, str, x, y,         text_color, adjust)


def blit_text(screen, font, str, x, y, color, adjust = "left"):
    text = font.render(str, True, color)
    if adjust == "center":
        x -= text.get_width() // 2
    elif adjust == "right":
        x -= text.get_width()
    screen.blit(text, (x, y))
    return (text.get_width(), text.get_height())


def degree2str(degree):
  if degree <= 11.25:
    return "N"
  elif degree <= 33.75:
    return "NNE"
  elif degree <= 56.25:
    return "NE"
  elif degree <= 78.75:
    return "ENE"
  elif degree <= 101.25:
    return "E"
  elif degree <= 123.75:
    return "ESE"
  elif degree <= 146.25:
    return "SE"
  elif degree <= 168.75:
    return "SSE"
  elif degree <= 168.75:
    return "S"
  elif degree <= 213.75:
    return "SSW"
  elif degree <= 236.25:
    return "SW"
  elif degree <= 258.75:
    return "WSW"
  elif degree <= 281.25:
    return "V"
  elif degree <= 303.75:
    return "WNW"
  elif degree <= 326.25:
    return "NW"
  elif degree <= 348.75:
    return "NNW"
  else:
    return "N"


# Download image from url
def download_image(url):
    image = None
    loops = 4
    parsed_uri = urlparse(url)
    host = parsed_uri.netloc
    while not image and loops > 0:
        loops -= 1
        headers = {
            'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
            'Upgrade-Insecure-Requests': '1',
            'Host': host,
            'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.1 Safari/605.1.15',
            'Accept-Language': 'en-us',
            'Accept-Encoding': 'gzip, deflate',
            'Connection': 'keep-alive',
            'Proxy-Connection': 'keep-alive'
        }

        # Ignode urllib.error.URLError CERTIFICATE_VERIFY_FAILED
        ssl._create_default_https_context = ssl._create_unverified_context
        try:
            req = urllib.request.Request(url = url, headers = headers)
            opener = build_opener(HTTPCookieProcessor())
            handler = urllib.request.urlopen(req)
            image = handler.read()
        except urllib.error.HTTPError as e:
            logging.error("Exception occurred", exc_info = True)
            if "https" in url:
                logging.debug("Attempting http downgrade")
                url = url.replace("https", "http")
            else:
                image = None
                break
    return image


# Load image for given icao24 and url. If an image is found not matching the url,
# download the new url and remove the previous one. Images are saved as
# <icao24>-<md5(url)> regardless of extension.
# Returns a Pygame surface
def load_image(icao24, url):
    img_dir = config["DEFAULT"]["ImageDir"]
    fname = "%s/%s-%s" % (img_dir, icao24, hashlib.md5(url.encode('utf-8')).hexdigest())
    logging.debug("Loading %s" % fname)

    files = glob.glob("%s/%s-*" % (img_dir, icao24))
    for file in files:
        if file != fname:
            try:
                logging.debug("Removing old version %s" % file)
                os.remove(file)
            except:
                pass

    if not os.path.isfile(fname):
        image = download_image(url)
        try:
            file = open(fname, "wb+")
            file.write(image)
            file.close()
        except Exception as e:
            logging.error("Exception occurred", exc_info = True)
            try:
                os.remove(fname)
            except:
                pass

    if os.path.isfile(fname):
        try:
            return pygame.image.load(fname)
        except pygame.error as e:
            logging.error("Exception occurred", exc_info = True)
            return None
    else:
        return None


def fill_rect(x, y, width, height):
    s = pygame.display.get_surface()
    r = pygame.Rect(x, y, width, height)
    colour = 0, 0, 0
    s.fill(colour, r)


def main():
    global config
    global adsb_message
    try:
        url = None
        image = None
        parser = argparse.ArgumentParser(description="Graze at the sky")
        parser.add_argument("-v", "--verbose", help="Increase output verbosity", action="store_true")
        parser.add_argument("-c", "--config", action="store", help="Configuration file", default="sampleconfig.yml")
        args = parser.parse_args()

        config = configparser.ConfigParser()
        try:
            config.read(args.config, encoding='utf-8')
        except Exception as e:
            print("Failed to read config file: %s" % str(e))
            sys.exit(1)

        level = logging.DEBUG if args.verbose else logging.WARNING

        log_formatter = logging.Formatter('%(asctime)s %(levelname)s %(funcName)s(%(lineno)d) %(message)s')
        logFile = config["DEFAULT"]["TempDir"] + ("/skygrazer.log")
        try:
            my_handler = RotatingFileHandler(logFile, mode='a', maxBytes=100*1024, backupCount=1, encoding=None, delay=0)
        except FileNotFoundError:
            print("Failed to create %s" % logFile)
            sys.exit(1)

        my_handler.setFormatter(log_formatter)
        app_log = logging.getLogger()
        app_log.addHandler(my_handler)
        app_log.setLevel(level)

        if args.verbose:
            consoleHandler = logging.StreamHandler()
            consoleHandler.setFormatter(log_formatter)
            app_log.addHandler(consoleHandler)

        logging.debug('---------------------------------------------------------')
        logging.debug('Skygrazer started')

        screen = pygame_init()
        clock = pygame.time.Clock()

        white = (255, 255, 255)
        black = (0, 0, 0)
        if "font_size" in config["UI"]:
            font_size = int(config["UI"]["font_size"])
        else:
            font_size = 72

        last_message = datetime.datetime.now().timestamp()

        font = pygame.font.Font("ubuntu-font-family-0.83/Ubuntu-C.ttf", font_size)
        font_height = font.render("dg", True, white).get_height()
        arrows = pygame.font.Font("arrows/Arrows.otf", int(font_size * 1.25))

        info = pygame.display.Info()

        if "screen_width" in config["UI"]:
            screen_width = int(config["UI"]["screen_width"])
        else:
            screen_width = info.current_w
        if "screen_height" in config["UI"]:
            screen_height = int(config["UI"]["screen_height"])
        else:
            screen_height = info.current_h

        bridge = mybridge(host = config["MQTT"]["MQTTBroker"], mqtt_topic = config["MQTT"]["MQTTProximityTopic"], port = 1883, client_id = "skygrazer-%d" % (random.randint(0, 65535)), user_id = None, password = None)

        # Show splash screen
        image = pygame.image.load("splash.png")
        if image:
            (w, h) = image.get_size()
            ratio = w / h
            w = screen_width
            h = w / ratio
            image = pygame.transform.smoothscale(image, (w, int(h)))
            screen.blit(image, (0, 0))
            pygame.display.flip()
            time.sleep(4)

        while True:
            bridge.looping()

            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    sys.exit(0)
                if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                    sys.exit(0)

            # Timeout
            if datetime.datetime.now().timestamp() - last_message > 60:
                screen.fill((0, 0, 0))

            if adsb_message:
                last_message = datetime.datetime.now().timestamp()
                screen.fill((0, 0, 0))

                if adsb_message.image:
                    if adsb_message.image != url:
                        url = adsb_message.image
                        image = load_image(adsb_message.icao24, url)
                        if image:
                            (w, h) = image.get_size()
                            ratio = w / h
                            w = screen_width
                            h = w / ratio
                            image = pygame.transform.smoothscale(image, (w, int(h)))
                else:
                    image = None
                    url = None

                if image:
                    screen.blit(image, (0, 0))

                margin = 10
                y = screen_height - font_height
                dist_x = margin
                bearing_x = screen_width * 0.15
                alt_x = screen_width * 1/3
                spd_x = screen_width * 2/3
                hdg_x = screen_width - margin

                if 1: # Clear lower area for flight data
                    fill_rect(0, screen_height - font_height, screen_width, font_height)

                if adsb_message.type:
                    blit_outlined_text(screen, font, adsb_message.type, margin, 0, white, black)

                    try:
                        route = "%s > %s" % (adsb_message.route.origin.city, adsb_message.route.destination.city)
                        blit_outlined_text(screen, font, route, hdg_x + 1, 1, white, black, "right")
                    except AttributeError as e:
                        logging.warning(e)
                        pass

                if adsb_message.distance < 1:
                    blit_text(screen, font, "%d m %s" % (1000 * adsb_message.distance, degree2str(adsb_message.bearing)), dist_x, y, white)
                else:
                    blit_text(screen, font, "%.1f km %s" % (adsb_message.distance, degree2str(adsb_message.bearing)), dist_x, y, white)

                if adsb_message.speed:
                    blit_text(screen, font, "%d kt" % adsb_message.speed, spd_x, y, white, "center")
                if adsb_message.altitude:
                    (tx, ty) = blit_text(screen, font, "%d ft" % adsb_message.altitude, alt_x, y, white, "center")
                    if adsb_message.vspeed and abs(adsb_message.vspeed) > 150:
                        if adsb_message.vspeed > 0:
                            blit_text(screen, arrows, "C", alt_x + tx/2 + 10, y, white)
                        else:
                            blit_text(screen, arrows, "D", alt_x + tx/2 + 10, y, white)
                if adsb_message.heading:
                    blit_text(screen, font, "Hdg %s" % degree2str(adsb_message.heading), hdg_x, y, white, "right")

                adsb_message = None

            pygame.display.flip()
            clock.tick(500)


    except Exception as e:
        logging.error("Exception occurred", exc_info = True)
#        pushover_publish(title = config["PUSHOVER"]["PushoverTitle"], message = "Appliance Monitor crashed: %s %s" % (str(e), tb))


if __name__ == "__main__":
    main()
