#
# Stupid 'expr' command can't handle float values and not everyone has installed 'bc'
# so calculate elapsed minutes here
# sz.

import sys
print (int(sys.argv[1]) - int(sys.argv[2])) * 0.0166666
