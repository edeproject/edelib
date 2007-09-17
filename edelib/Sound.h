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
 * \enum SoundDriver
 * \brief Driver types for SoundSystem class
 */
enum SoundDriver {
	SDR_NULL = 0,        ///< Null driver; no sound (of course)
	SDR_DEFAULT,         ///< System default driver
	SDR_ALSA,            ///< ALSA driver; works only with ALSA 0.5.x
	SDR_ALSA_09,         ///< ALSA driver with 0.9.x and 1.0.x API
	SDR_ESD,             ///< Driver for Enlightenment sound daemon
	SDR_OSS,             ///< OSS driver
	SDR_SUN,             ///< Sun audio driver
	SDR_AIX              ///< AIX audio driver
};

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
		unsigned long int thread_id;
		int thread_running;
		char pcm_out[PCM_BUF_SIZE];
#endif 
		SoundSystem();
		~SoundSystem();

		bool setup_driver(int driver);

		SoundSystem(const SoundSystem&);
		SoundSystem& operator=(SoundSystem&);
	public:
#ifndef SKIP_DOCS
		bool play_stream(const char* fname);
		bool play_stream_in_background(const char* fname);
		void stop_playing(void);
		bool stream_playing(void) { return thread_running; }
		static SoundSystem* instance(void);
#endif
		/**
		 * Initialize internal data and device using desired driver (list is in SoundDriver).
		 * Return false if initialization fails. This function <b>must</b>
		 * be called before SoundSystem::play().
		 */
		static bool init(SoundDriver driver = SDR_DEFAULT);

		/**
		 * Clears internal data and close device.
		 */
		static void shutdown(void);

		/**
		 * Return true if init() was called, but not shutdown().
		 * Otherwise return false.
		 */
		static bool inited(void);

		/**
		 * Plays given file. Ogg format is assumed.
		 *
		 * \return true if all went fine, otherwise false
		 * \param fname is full path to file
		 * \param block if set to true (default), it will play file
		 * in blocking mode, stopping program flow until playing was finished
		 */
		static bool play(const char* fname, bool block = 1);

		/**
		 * Only valid if play() is called in non-blocking mode. You should
		 * call this function periodically to check if file playing was finished.
		 */
		static bool playing(void);

		/**
		 * Only valid if play() is called in non-blocking mode. It will stop
		 * playing current stream.
		 */
		static void stop(void);
};

EDELIB_NS_END
#endif // __SOUND_H__
