dnl
dnl $Id$
dnl
dnl Part of Equinox Desktop Environment (EDE).
dnl Copyright (c) 2000-2007 EDE Authors.
dnl 
dnl This program is licenced under terms of the 
dnl GNU General Public Licence version 2 or newer.
dnl See COPYING for details.

dnl Enable sound param and check for facility
dnl For now only .ogg via libao
AC_DEFUN([EDE_SOUND], [

	AC_ARG_ENABLE(sounds, [  --disable-sounds        disable sound support],, enable_sounds=yes)

	if eval "test $enable_sounds = yes"; then
		dnl checks for libao
		AC_CHECK_HEADER(ao/ao.h, [have_ao_h=yes], [have_ao_h=no])
		AC_CHECK_LIB(ao, ao_is_big_endian, [have_ao_lib=yes], [have_ao_lib=no])
   
		dnl checks for libvorbis
		AC_CHECK_HEADER(vorbis/codec.h, [have_codec_h=yes], [have_codec_h=no])
		AC_CHECK_LIB(vorbis, vorbis_info_init, [have_vorbis_lib=yes], [have_vorbis_lib=no])

		dnl checks for libvorbisfile
		AC_CHECK_HEADER(vorbis/vorbisfile.h, [have_vorbisfile_h=yes], [have_vorbisfile_h=no])
		AC_CHECK_LIB(vorbisfile, ov_clear, [have_vorbisfile_lib=yes], [have_vorbisfile_lib=no])

		dnl checks for libmad
		AC_CHECK_HEADER(mad.h, [have_mad_h=yes], [have_mad_h=no])
		AC_CHECK_LIB(mad, mad_decoder_finish, [have_mad_lib=yes], [have_mad_lib=no])

		if eval "test $have_vorbis_lib = yes" && eval "test $have_vorbisfile_lib = yes"; then
			sresult="ogg"
			slibs="-lvorbis -lvorbisfile"
			AC_DEFINE(USE_OGG, 1, [Define to 1 if you want to enable ogg decoder for sounds])
		fi

		if eval "test $have_mad_lib = yes"; then
			sresult="$sresult mp3"
			slibs="$slibs -lmad"
			AC_DEFINE(USE_MAD, 1, [Define to 1 if you want to enable mp3 decoder for sounds via libmad])
		fi

		AC_MSG_CHECKING(sound support)
		if eval "test $have_ao_lib = yes"; then
				AC_DEFINE(USE_SOUNDS, 1, [Define to 1 if you want to enable sound support])
				AC_MSG_RESULT([ok, with $sresult])
				SOUNDLIBS="-lao $slibs"
		else
			AC_MSG_RESULT(disabled)
		fi
   	fi
])
