/*
 * $Id$
 *
 * Sound system facility
 * Part of edelib.
 * Copyright (c) 2000-2007 EDE Authors.
 *
 * This program is licenced under terms of the
 * GNU General Public Licence version 2 or newer.
 * See COPYING for details.
 */

#ifndef __SOUND_H__
#define __SOUND_H__

#include "econfig.h"

#ifdef USE_SOUNDS
	#include <ao/ao.h>
#endif

#define PCM_BUF_SIZE 4096

EDELIB_NAMESPACE {

/**
 * \class SoundSystem
 * SoundSystem is a class responsible for playing sound files.
 *
 * This class depends on libao and if during configure phase this
 * library is not detected, the class will do nothing.
 *
 * For now the only supported format is ogg, but for the future is
 * planned mp3 and wav.
 *
 * Before it's usage, init() method should be called or assertion will
 * be triggered. This could be avoided setting init() method inside
 * constructor, but the main reason why is not is because on demand loading
 * so initialization does not have to depend on it's instance definition.
 */

class SoundSystem
{
#ifdef USE_SOUNDS
	private:
		ao_device* device;
		ao_sample_format format;
		int default_driver;
		char pcm_out[PCM_BUF_SIZE];
		bool inited;
		bool down;
#endif 
	public:
		/**
		 * Empty constructor that will NULL-ify internal data
		 */
		SoundSystem();

		/**
		 * Destructor will close all currently opened devices, if needed
		 */
		~SoundSystem();

		/**
		 * This must be called before playing any type of sound
		 */
		void init(void);

		/**
		 * Explicitly close all opened devices. Calling play() method after this will
		 * trigger assertion
		 */
		void shutdown(void);

		/**
		 * Play content of fname. Assumed format is ogg vorbis, and if file can't be
		 * opened, or errors during playing stream occurs, it will return 0
		 */
		int play(const char* fname);
};
}
#endif
