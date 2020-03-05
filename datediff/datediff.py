import sys
from datetime import datetime, timedelta

argc = len(sys.argv)
if argc != 2 and argc != 3:
        print('Usage: python datediff.py <from date> [to date]')
        print('e.g. $ python datediff.py 19860213')
        print('e.g. $ python datediff.py 20120101 20200217')
        exit()
if argc == 2:
        to_date = datetime.now()
elif argc == 3:
        to_date = datetime.strptime(sys.argv[2], '%Y%m%d')
from_date = datetime.strptime(sys.argv[1], '%Y%m%d')
print((to_date - from_date).days)
