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
		SoundSystem();
		~SoundSystem();
		void init(void);
		void shutdown(void);

		int play(const char* fname);
};
}
#endif
