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

#ifdef USE_SOUNDS
	#include <vorbis/vorbisfile.h>
	#include <vorbis/codec.h>
#endif

EDELIB_NAMESPACE {

SoundSystem::SoundSystem()
{
#ifdef USE_SOUNDS
	device = NULL;
	inited = false;
	down = false;
#endif
}

SoundSystem::~SoundSystem()
{
#ifdef USE_SOUNDS
	if(!down)
		shutdown();		// just in case
#endif
}

void SoundSystem::init(void)
{
#ifdef USE_SOUNDS
	EDEBUG("Loading sound system\n");

	ao_initialize();
	default_driver = ao_default_driver_id();
	inited = true;
#endif
}

void SoundSystem::shutdown(void)
{
#ifdef USE_SOUNDS
	EDEBUG("Shutting down sound system\n");

	ao_shutdown();
	down = true;
#endif
}

int SoundSystem::play(const char* fname)
{
#ifdef USE_SOUNDS
	EASSERT(inited != false);
	EASSERT(fname != NULL);

	FILE* f = fopen(fname, "rb");
	if(f == NULL)
	{
		EWARNING("Can't open %s\n", fname);
		return 0;
	}

	OggVorbis_File vf;
	vorbis_info* vi;

	if(ov_open(f, &vf, NULL, 0) < 0)
	{
		EWARNING("%s does not appear to be ogg file\n");
		fclose(f);
		return 0;
	}

	// read and print comments
	char** comm = ov_comment(&vf, -1)->user_comments;
	vi = ov_info(&vf, -1);

	while(*comm)
	{
		EDEBUG("%s", *comm);
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
	//device = ao_open_live(ao_driver_id("oss"), &format, NULL);
	if(device == NULL)
	{
		EWARNING("Can't open device");
		ov_clear(&vf);
		return 0;
	}

	int current_section;
	while(1)
	{
		long ret = ov_read(&vf, pcm_out, sizeof(pcm_out), 0, 2, 1, &current_section);
		if(ret == 0)
			break;
		else if(ret < 0)
			EDEBUG("Error in the stream, continuing...");
		else
			ao_play(device, pcm_out, ret);
	}

	ao_close(device);
	device = NULL;

	// NOTE: fclose(f) is not needed, since ov_clear() will close file
	ov_clear(&vf);
#endif	// USE_SOUNDS

	return 1;
}

}
