#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import re
import os.path
import pytz
from datetime import datetime
from gnuradio.blocks import parse_file_metadata
import sys
from math import floor
import pmt

def parse_filename(filename):
    re_filenames = (
        re.compile('^(?P<prefix>[-_\.a-z0-9]+?)-(?P<datestr>[0-9]{4}-[0-9]{2}-[0-9]{2}-[0-9]{2}-[0-9]{2}-[0-9]{2}Z?)(?:-(?P<OPnumber>OP[0-9]+))?-(?P<suffix>.+?)(?:[-\.](?P<sampratestr>[0-9]*[pnumkMGTP][0-9]*))?\.(?P<ext>[a-zA-Z][a-zA-Z0-9]*)$', re.IGNORECASE),
    )
    _num_prefixes = {
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
    
    def _str2num(s):
        for exp, symbol in _num_prefixes.items():
            if symbol in s and symbol:
                if s.startswith(symbol):
                    a, b = '0', s[1:]
                elif s.endswith(symbol):
                    a, b = s[:-1], '0'
                else:
                    a, b = s.split(symbol)
                return (float(a)+float('0.'+b))*10**(exp)
        return float(s)
    
    filename_base = os.path.basename(filename)
    
    for rf in re_filenames:
        m = rf.match(filename_base)
        
        if m:
            d = m.groupdict()
            d['filename'] = filename_base
            
            if 'datestr' in d and d['datestr']:
                if d['datestr'].endswith('Z'):
                    starttime = pytz.UTC.localize(datetime.strptime(d['datestr'][:-1], '%Y-%m-%d-%H-%M-%S'))
                else:
                    local_zone = pytz.timezone('Europe/Berlin')
                    starttime = local_zone.localize(datetime.strptime(d['datestr'], '%Y-%m-%d-%H-%M-%S')).astimezone(pytz.UTC)

                d['date'] = starttime
            
            if 'sampratestr' in d and d['sampratestr']:
                d['samp_rate'] = _str2num(d['sampratestr'])
            
            return d
    
    raise ValueError('Cannot parse filename "{}"'.format(filename_base))

def write_metadata(header_file, samples_bytes, start_dt, samp_rate, extra_dict=None):
    METADATA_VERSION = 0x0
    dt = (start_dt - pytz.UTC.localize(datetime(1970, 1, 1, 0, 0, 0))).total_seconds()
    dt_secs = int(floor(dt))
    dt_frac = dt - dt_secs
    
    extra_dict_str = pmt.serialize_str(pmt.to_pmt(extra_dict)) if extra_dict else ''
    
    header = pmt.make_dict()
    header = pmt.dict_add(header, pmt.to_pmt("version"), pmt.to_pmt(METADATA_VERSION))
    header = pmt.dict_add(header, pmt.to_pmt("rx_rate"), pmt.from_float(samp_rate))
    header = pmt.dict_add(header, pmt.to_pmt("rx_time"), pmt.make_tuple(pmt.from_uint64(dt_secs), pmt.from_float(dt_frac)) )
    header = pmt.dict_add(header, pmt.to_pmt("size"), pmt.from_long(8)) # 2*32 bit = 8 byte / sample
    header = pmt.dict_add(header, pmt.to_pmt("type"), pmt.from_long(5)) # 5 = float
    header = pmt.dict_add(header, pmt.to_pmt("cplx"), pmt.PMT_T)
    header = pmt.dict_add(header, pmt.to_pmt("strt"), pmt.from_uint64(parse_file_metadata.HEADER_LENGTH+len(extra_dict_str)))
    header = pmt.dict_add(header, pmt.to_pmt("bytes"), pmt.from_uint64(samples_bytes))
    header_str = pmt.serialize_str(header)
    
    with open(header_file, 'wb') as f:
        f.write(header_str)
        if extra_dict:
            f.write(extra_dict_str)

    
def _test_parse_filename(name):
    print('parse_filename({}) = {}'.format(name, parse_filename(name)))

def _test():
  _test_parse_filename('MOVE-II-2021-01-12-19-40-13Z-OP77609535-dopplercorrected-25k.dat')
  _test_parse_filename('/tmmp/test/MOVE-II-2021-01-12-19-40-13Z-OP77609535-dopplercorrected-25k.dat')
  _test_parse_filename('MOVE-II-2021-01-12-19-40-13Z-OP77609535-with-doppler-25k.dat')
  _test_parse_filename('MOVE-II-2021-01-12-19-40-13Z-OP77609535-with-doppler-25k5.dat')
  _test_parse_filename('MOVE-II-2021-01-12-19-40-13Z-OP77609535-with-doppler-M5.dat')
  _test_parse_filename('MOVE-II-2021-01-12-19-40-13Z-OP77609535-binary.bin')
  _test_parse_filename('MOVE-II-FM-2019-08-08-10-29-41-baseband_nogui.250k.dat')
  _test_parse_filename('MOVE-II-FM-2019-08-08-10-29-41-decoded_nogui.bin')
  _test_parse_filename('MOVE-II-FM-2019-08-08-10-29-41-w_doppler_nogui.25k.dat')
  _test_parse_filename('MOVE-II-FM-2019-08-08-10-29-41-wo_doppler_nogui.25k.dat')
  _test_parse_filename('MOVE-II-FM-2019-08-08-10-29-41-wo_doppler_nogui.25k5.dat')
  _test_parse_filename('MOVE-II-FM-2019-08-08-10-29-41-wo_doppler_nogui.M25.dat')

if __name__ == '__main__':
    if '--help' in sys.argv or '-h' in sys.argv:
        print('Usage: {} <recording file> [<recording file> ...]\n\nParse filename of provided files and generate a metadata file with the same name, but a .hdr extension containing the recording time, sampling rate, satellite name, overpass number and file suffix in GNURadio metadata format.\n\nThe filename must have the following format "<Satellite>-<date>-OP<overpass number>-suffix[-.]<samp rate>.<ext>". The date needs to have the format "YYYY-MM-DD-HH-MM-SS(Z?)"'.format(sys.argv[0]))
        sys.exit(0)
        
    for file_in in sys.argv[1:]:
        if not os.path.isfile(file_in):
            print('File "{}" does not exist.'.format(file_in))
            sys.exit(1)
        
        file_size = os.path.getsize(file_in)
        header_name = file_in + '.hdr'
        
        file_metadata = parse_filename(file_in)
        
        write_metadata(header_name, file_size, file_metadata['date'], file_metadata['samp_rate'], {
                'satellite': file_metadata['prefix'],
                'overpass':  file_metadata['OPnumber'],
                'suffix':    file_metadata['suffix'],
                'filename':  file_metadata['filename'],
            }
        )
