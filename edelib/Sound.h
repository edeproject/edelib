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

EDELIB_NS_BEGIN

/**
 * \class SoundSystem
 * \brief A class responsible for playing sound files.
 *
 * This class depends on libao and if during configure phase this
 * library is not detected, the class will do nothing.
 *
 * For now the only supported format is ogg, but for the future is
 * planned mp3 and wav.
 *
 * Before it's usage, SoundSystem::init() <b>must</b> be called or assertion will
 * be triggered if SoundSystem::play() call is attempted. SoundSystem::init()
 * usually should be placed before application loads gui data, and SoundSystem::shutdown()
 * when sound will not be played.
 */

class EDELIB_API SoundSystem {
	private:
		static SoundSystem* pinstance;

#ifdef USE_SOUNDS
		ao_device* device;
		ao_sample_format format;
		int default_driver;
		char pcm_out[PCM_BUF_SIZE];
#endif 
		SoundSystem();
		~SoundSystem();

		SoundSystem(const SoundSystem&);
		SoundSystem& operator=(SoundSystem&);
	public:
#ifndef SKIP_DOCS
		bool play_stream(const char* fname);
		static SoundSystem* instance(void);
#endif
		/**
		 * Initialize internal data and device. This function <b>must</b>
		 * be called before SoundSystem::play().
		 */
		static void init(void);

		/**
		 * Clears internal data and close device.
		 */
		static void shutdown(void);

		/**
		 * Plays given file. Ogg format is assumed.
		 *
		 * \return true if all went fine, otherwise false
		 * \param  fname is full path to file
		 */
		static bool play(const char* fname);
};

EDELIB_NS_END
#endif // __SOUND_H__
