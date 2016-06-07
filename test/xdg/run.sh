#/bin/sh

compare_icon_theme_results() {
	theme_name="edeneu"
	icon_name=$1
	icon_size=$2

	ret1=`./icon-theme $theme_name $icon_name $icon_size 2> /dev/null`
	ret2=`python icon-theme.py $theme_name $icon_name $icon_size`

	if test "x$ret1" != "x$ret2"; then
		echo "icon-theme: FAILED: $ret1 != $ret2"
	fi
}

compare_icon_theme_results "folder" 16
compare_icon_theme_results "folder" 32
compare_icon_theme_results "gaim"   32
compare_icon_theme_results "folder-visiting" 64
compare_icon_theme_results "dialog-warning" 128

# on my system these are 'hicolor', installed by KDE
compare_icon_theme_results "konqueror" 16
compare_icon_theme_results "amarok"    128
