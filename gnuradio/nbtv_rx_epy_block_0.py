"""
Embedded Python Blocks:

Each time this file is saved, GRC will instantiate the first class it finds
to get ports and parameters of your block. The arguments to __init__  will
be the parameters. All of them are required to have default values!
"""

import numpy as np
from gnuradio import gr
import pygame

class blk(gr.sync_block):  # other base classes are basic_block, decim_block, interp_block
    """Embedded Python Block example - a simple multiply const"""

    def __init__(self, lines=32, line_size=48, framerate=12.5, sync_level=0.05):  # only default arguments here
        """arguments to this function show up as parameters in GRC"""
        gr.sync_block.__init__(
            self,
            name='NBTV Receive',   # will show up in GRC
            in_sig=[np.float32],
            out_sig=[]
        )
        # if an attribute with the same name as a parameter is found,
        # a callback is registered (properties work, too).
        self.lines = line_size
        self.line_size = lines
        self.framerate = framerate
        self.sync_level = sync_level

        # Scanning beam
        self.lineCounter = 0
        self.pixelCounter = 0
        self.txCounterOffset = 0
        self.frameBuf = np.zeros((self.lines*self.line_size), dtype=np.uint8)
        self.synced = False

        self.started = False

    def work(self, input_items, output_items):
        """example: multiply with constant"""
        
        # Initialize display
        if(self.started == False):
            pygame.init()
            self.screen = pygame.display.set_mode((self.line_size*8, self.lines*8))
            self.started = True

        input_items_orig = np.copy(input_items)
        np.rint(input_items[0]*340, out=input_items[0])
        for i in range(0, len(input_items[0])):
            #self.frameBuf[(self.lineCounter*self.line_size)+self.pixelCounter] = input_items[0][i]
            if(input_items_orig[0][i] <= self.sync_level):
                self.pixelCounter = 0
                self.lineCounter += 1
                if(self.lineCounter >= self.lines):
                    self.lineCounter = 0
                    pygame.display.flip()
                continue
            
            #  Draw the current pixel
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
            brightness = int(input_items[0][i])
            if brightness < 0:
                brightness = 0
            if brightness > 255:
                brightness = 255
            pygame.draw.rect(self.screen, (brightness, brightness, brightness), (self.pixelCounter*8, self.lineCounter*8, 8, 8))
            #pygame.display.flip()

            # Move the beam along
            self.pixelCounter += 1
            if(self.pixelCounter >= self.line_size):
                self.pixelCounter = 0
                self.lineCounter += 1
                if(self.lineCounter >= self.lines):
                    self.lineCounter = 0
                    pygame.display.flip()

        return len(input_items[0])
