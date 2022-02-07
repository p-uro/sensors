#!/usr/bin/env python

from __future__ import print_function
import sys, random, libmapper as mpr

src = mpr.Device("py.testmapfromstr.src")
outsig = src.add_signal(mpr.Direction.OUTGOING, "outsig", 1, mpr.Type.INT32)

dest = mpr.Device("py.testmapfromstr.dst")
insig = dest.add_signal(mpr.Direction.INCOMING, "insig", 1, mpr.Type.FLOAT,
                        None, None, None, None,
                        lambda s, e, i, v, t: print('signal', s['name'], 'got value',
                                                    v, 'at time', t.get_double()),
                        mpr.Signal.Event.UPDATE)

while not src.ready or not dest.ready:
    src.poll(10)
    dest.poll(10)

map = mpr.Map("%y=(%x+100)*2", insig, outsig)
map.push()

while not map.ready:
    src.poll(10)
    dest.poll(10)

for i in range(100):
    outsig.set_value(i)
    dest.poll(10)
    src.poll(0)
