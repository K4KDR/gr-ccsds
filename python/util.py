#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2018 Martin Luelf.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

import math

def getTleFromFile(filename, satname):
  out = []
  with open(filename, 'r') as f:
    flag = False
    satname_clean = str(satname).rstrip().lower()
    
    for line in f:
      if line.rstrip().lower() == satname_clean:
        flag = True
      
      if flag:
        if len(out) >= 3:
          return out
        out.append(line.rstrip())
    
  raise KeyError('Cannot find satellite \'%s\' in %s' % (satname, filename))

def num2str(n):
  prefixes = {
    -12: 'p',
    -9: 'n',
    -6: 'u',
    -3: 'm',
    0: '',
    3: 'k',
    6: 'M',
    9: 'G',
    12: 'T',
    15: 'P',
    }
  
  min_factor = min(prefixes.keys())
  max_factor = max(prefixes.keys())
  
  e = 3*math.floor(math.log10(float(n))/3.0)
  
  if e < min_factor:
    factor = min_factor
  elif e > max_factor:
    factor = max_factor
  elif e in prefixes:
    factor = e
  else:
    raise ValueError('Cannot handle a value of e=%f' % e)
  
  label = prefixes[factor]
  number = n/(10**factor)
  number_str = ('%f' % number).rstrip('0').rstrip('.')
  return number_str+label

def _run(n):
  print('num2str(%f) = %s' % (n, num2str(n)))
    
if __name__ == '__main__':
  _run(0.11)
  _run(1)
  _run(15)
  _run(500)
  _run(1500)
  _run(999)
  _run(99999)
  _run(125.6e6)
  _run(125.6e15)
  _run(0.234e-15)
  _run(1.234e-15)
