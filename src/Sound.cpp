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

#include <edelib/Sound.h>
#include <edelib/econfig.h>
#include <edelib/Debug.h>

#include "ThreadableMember.h"

#ifdef USE_SOUNDS
	#include <vorbis/vorbisfile.h>
	#include <vorbis/codec.h>
#endif

EDELIB_NS_BEGIN

SoundSystem* SoundSystem::pinstance = NULL;

SoundSystem::SoundSystem() {
#ifdef USE_SOUNDS
	default_driver = -1;
	thread_id = 0;
	thread_running = 0;

	device = NULL;
	ao_initialize();

	EDEBUG(ESTRLOC ": SoundSystem init\n");
#endif
}

SoundSystem::~SoundSystem() {
#ifdef USE_SOUNDS
	if(thread_running) {
		EDEBUG(ESTRLOC ": Stopping running thread\n");
		StopThread(thread_id);
	}

	ao_shutdown();
	EDEBUG(ESTRLOC ": SoundSystem shutdown\n");
#endif
}

bool SoundSystem::setup_driver(int driver) {
#ifdef USE_SOUNDS
	const char* name;
	// names must be libao compatible
	switch(driver) {
		case SDR_NULL:
			name = "null";
			break;
		case SDR_ALSA:
			name = "alsa";
			break;
		case SDR_ALSA_09:
			name = "alsa09";
			break;
		case SDR_ESD:
			name = "esd";
			break;
		case SDR_OSS:
			name = "oss";
			break;
		case SDR_SUN:
			name = "sun";
			break;
		case SDR_AIX:
			name = "aixs";
			break;
		case SDR_DEFAULT:
		default: 
			name = 0; // let NULL means default
	}

	if(!name)
		default_driver = ao_default_driver_id();
	else
		default_driver = ao_driver_id(name);

	/* 
	 * assume: if we can't get device info, something is wrong
	 * and disable further playing 
	 */
	bool ret = (default_driver != -1);
	ao_info* ai = ao_driver_info(default_driver);

	if(ret && ai)
		EDEBUG(ESTRLOC ": Playing with %s (%s) driver\n", ai->name, ai->short_name);
	else
		EWARNING(ESTRLOC ": Can't get device info. Playing nothing\n");

	return ret;
#else // USE_SOUNDS

	// FIXME: return true or false ???
	return false;

#endif // USE_SOUNDS
}

bool SoundSystem::init(SoundDriver driver) {
	if(SoundSystem::pinstance != NULL)
		return true;

	SoundSystem::pinstance = new SoundSystem();
	return SoundSystem::pinstance->setup_driver(driver);
}

void SoundSystem::shutdown(void) {
	if(SoundSystem::pinstance == NULL)
		return;

	delete SoundSystem::pinstance;
	SoundSystem::pinstance = NULL;
}

bool SoundSystem::inited(void) {
	return (SoundSystem::pinstance != NULL);
}

SoundSystem* SoundSystem::instance(void) {
	EASSERT(SoundSystem::pinstance != NULL && "Did you run SoundSystem::init() ?");
	return SoundSystem::pinstance;
}

bool SoundSystem::play(const char* fname, bool block) {
#ifdef USE_SOUNDS
	int ret;
	if(block)
		ret = SoundSystem::instance()->play_stream(fname);
	else
		ret = SoundSystem::instance()->play_stream_in_background(fname);

	return ret;
#else
	return false;
#endif
}

bool SoundSystem::playing(void) {
#ifdef USE_SOUNDS
	return SoundSystem::instance()->stream_playing();
#else
	return false;
#endif
}

void SoundSystem::stop(void) {
	SoundSystem::instance()->stop_playing();
}

void SoundSystem::stop_playing(void) {
#ifdef USE_SOUNDS
	if(thread_running) {
		StopThread(thread_id);
		thread_running = 0;
	}
#endif
}

bool SoundSystem::play_stream_in_background(const char* fname) {
#ifdef USE_SOUNDS
	// stop any running threads
	stop_playing();

	SoundSystem* si = SoundSystem::instance();

	if(!RunInThread(*si, &SoundSystem::play_stream, fname, thread_id, &thread_running)) {
		thread_running = false;
		return false;
	}

	thread_running = true;
#endif
	return true;
}

bool SoundSystem::play_stream(const char* fname) {
#ifdef USE_SOUNDS
	EASSERT(fname != NULL);

	// assure we don't play with unopened device
	if(default_driver == -1)
		return false;

	FILE* f = fopen(fname, "rb");
	if(f == NULL) {
		EWARNING("Can't open %s\n", fname);
		return false;
	}

	OggVorbis_File vf;
	if(ov_open(f, &vf, NULL, 0) < 0) {
		EWARNING(ESTRLOC ": %s does not appear to be ogg file\n");
		fclose(f);
		return false;
	}

	// read and print comments
	char** comm = ov_comment(&vf, -1)->user_comments;
	vorbis_info* vi = ov_info(&vf, -1);

	while(*comm) {
		EDEBUG(ESTRLOC ": %s", *comm);
		comm++;
	}

	EASSERT(vi != NULL);

	/*
	 * TODO: should be word_size * 8
	 * Check this more. In previous version
	 * using 4 * 8 bits worked (impossible!)
	 */
	format.bits = 16;	
	format.channels = vi->channels;
	format.rate = vi->rate;
	format.byte_format = AO_FMT_NATIVE;

	// now open device
	device = ao_open_live(default_driver, &format, NULL);
	if(device == NULL) {
		EWARNING(ESTRLOC ": Can't open device\n");
		ov_clear(&vf);
		return false;
	}

	int current_section;
	while(1) {
		long ret = ov_read(&vf, pcm_out, sizeof(pcm_out), 0, 2, 1, &current_section);

		if(ret == 0)
			break;
		else if(ret < 0)
			EDEBUG(ESTRLOC ": Error in the stream, continuing...");
		else
			ao_play(device, pcm_out, ret);
	}

	ao_close(device);
	device = NULL;
	// NOTE: fclose(f) is not needed, since ov_clear() will close file
	ov_clear(&vf);

#endif
	return true;
}

EDELIB_NS_END
