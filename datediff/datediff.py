import os
import sys

argc = len(sys.argv)
if argc != 2 and argc != 3:
        print('Usage: python datediff.py <from date> [to date]')
        print('e.g. $ python datediff.py 19860213')
        print('e.g. $ python datediff.py 20120101 2020021')
        exit()
if argc == 2:
        to_ts = int(os.popen('date +%s').read())
elif argc == 3:
        to_ts = int(os.popen('date -d"' + sys.argv[2] + '" +%s').read())
from_ts = int(os.popen('date -d"' + sys.argv[1] + '" +%s').read())
diff = (to_ts - from_ts) // (3600 * 24)
print(diff)
