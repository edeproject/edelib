import xdg.IconTheme
import xdg.Config
import sys

if len(sys.argv) != 4:
	print "Usage: %s <icon-theme> <icon-name> <icon-size>" % sys.argv[0]
else:
	xdg.Config.setIconTheme(sys.argv[1])
	ret = xdg.IconTheme.getIconPath(sys.argv[2], int(sys.argv[3]))
	if ret == None:
		print ""
	else:
		print ret
