#
# Regular cron jobs for the xlxd package.
#
0 4	* * *	root	[ -x /usr/bin/xlxd_maintenance ] && /usr/bin/xlxd_maintenance
