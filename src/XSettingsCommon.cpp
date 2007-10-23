/*
 * $Id$
 *
 * Common stuff for XSETTINGS protocol
 * Part of edelib.
 * Based on implementation from Owen Tylor, copyright (c) 2001 Red Hat, inc.
 * Copyright (c) 2005-2007 EDE Authors.
 *
 * This program is licensed under terms of the 
 * GNU General Public License version 2 or newer.
 * See COPYING for details.
 */

#include <edelib/XSettingsCommon.h>
#include <string.h>  // strcmp, memcpy
#include <stdio.h>   // puts

#include <X11/Xmd.h> // CARDX
#include <X11/X.h>   // MSBFirst, LSBFirst

#define BYTES_LEFT(buffer) ((buffer)->data + (buffer)->len - (buffer)->pos)

EDELIB_NS_BEGIN

bool xsettings_list_add(XSettingsList** list, XSettingsSetting* setting) {
	XSettingsList* node;
	XSettingsList* iter;
	XSettingsList* last = NULL;

	node = new XSettingsList;
	node->setting = setting;

	iter = *list;
	while(iter) {
		int cmp = strcmp(setting->name, iter->setting->name);
		if(cmp < 0)
			break;
		else if(cmp == 0) {
			delete node;
			puts("Found duplicate entry");
			return false;
		}

		last = iter;
		iter = iter->next;
	}

	if(last)
		last->next = node;
	else
		*list = node;
	node->next = iter;

	return true;
}

void xsettings_setting_free(XSettingsSetting* setting) {
	if(!setting)
		return;
	if(setting->type == XSETTINGS_TYPE_STRING)
		delete [] setting->data.v_string;

	if(setting->name)
		delete [] setting->name;
	delete setting;
}

XSettingsSetting* xsettings_list_find(XSettingsList* list, const char* name) {
	XSettingsList* iter = list;
	while(iter) {
		if(strcmp(iter->setting->name, name) == 0)
			return iter->setting;

		iter = iter->next;
	}

	return NULL;
}

bool xsettings_list_remove(XSettingsList** list, const char* name) {
	XSettingsList* iter;
	XSettingsList* last = NULL;

	iter = *list;
	while(iter) {
		if(strcmp(iter->setting->name, name) == 0) {
			if(last)
				last->next = iter->next;
			else
				*list = iter->next;
			xsettings_setting_free(iter->setting);
			delete iter;
			return true;
		}

		last = iter;
		iter = iter->next;
	}

	return false;
}

void xsettings_list_free(XSettingsList* list) {
	XSettingsList* next;

	while(list) {
		next = list->next;
		xsettings_setting_free(list->setting);
		delete list;
		list = next;
	}
}

XSettingsSetting* xsettings_setting_copy(XSettingsSetting* setting) {
	XSettingsSetting* ret = new XSettingsSetting;

	int len = strlen(setting->name);
	ret->name = new char[len + 1];
	memcpy(ret->name, setting->name, len + 1);

	ret->type = setting->type;

	switch(setting->type) {
		case XSETTINGS_TYPE_INT:
			ret->data.v_int = setting->data.v_int;
			break;
		case XSETTINGS_TYPE_COLOR:
			ret->data.v_color = setting->data.v_color;
			break;
		case XSETTINGS_TYPE_STRING:
			len = strlen(setting->data.v_string);
			ret->data.v_string = new char[len + 1];
			memcpy(ret->data.v_string, setting->data.v_string, len + 1);
			break;
	}

	ret->last_change_serial = setting->last_change_serial;
	return ret;
}

bool xsettings_setting_equal(XSettingsSetting* s1, XSettingsSetting* s2) {
	if(s1->type != s2->type)
		return false;

	if(strcmp(s1->name, s2->name) != 0)
		return false;

	switch(s1->type) {
		case XSETTINGS_TYPE_INT:
			return s1->data.v_int == s2->data.v_int;
		case XSETTINGS_TYPE_COLOR:
			return (s1->data.v_color.red == s2->data.v_color.red &&
					s1->data.v_color.green == s2->data.v_color.green &&
					s1->data.v_color.blue == s2->data.v_color.blue &&
					s1->data.v_color.alpha == s2->data.v_color.alpha);
		case XSETTINGS_TYPE_STRING:
			return (strcmp(s1->data.v_string, s2->data.v_string) == 0);
	}

	return false;
}

int xsettings_setting_len(const XSettingsSetting* setting) {
	int len = 8; // type + pad + name-len + last-change-serial
	len += XSETTINGS_PAD(strlen(setting->name), 4);

	switch(setting->type) {
		case XSETTINGS_TYPE_INT:
			len += 4;
			break;
		case XSETTINGS_TYPE_COLOR:
			len += 8;
			break;
		case XSETTINGS_TYPE_STRING:
			len += 4 + XSETTINGS_PAD(strlen(setting->data.v_string), 4);
			break;
	}

	return len;
}

char xsettings_byte_order(void) {
	CARD32 myint = 0x01020304;
	return (*(char*)&myint == 1) ? MSBFirst : LSBFirst;
}

static bool fetch_card8(XSettingsBuffer* buffer, CARD8* result) {
	if(BYTES_LEFT(buffer) < 1)
		return false;

	*result = *(CARD8*)buffer->pos;
	buffer->pos += 1;
	return true;
}

static bool fetch_card16(XSettingsBuffer* buffer, CARD16* result, char local_byte_order) {
	CARD16 x;

	if(BYTES_LEFT(buffer) < 2)
		return false;

	x = *(CARD16*)buffer->pos;
	buffer->pos += 2;

	if(buffer->byte_order == local_byte_order)
		*result = x;
	else
		*result = (x << 8) | (x >> 8);

	return true;
}

static bool fetch_ushort(XSettingsBuffer* buffer, unsigned short* result, char local_byte_order) {
	CARD16 x;

	if(fetch_card16(buffer, &x, local_byte_order)) {
		*result = x;
		return true;
	}

	return false;
}

static bool fetch_card32(XSettingsBuffer* buffer, CARD32* result, char local_byte_order) {
	CARD32 x;

	if(BYTES_LEFT(buffer) < 4)
		return false;

	x = *(CARD32*)buffer->pos;
	buffer->pos += 4;

	if(buffer->byte_order == local_byte_order)
		*result = x;
	else
		*result = (x << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | (x >> 24);
	return true;
}

XSettingsList* xsettings_decode(unsigned char* data, /*size_t*/ int len, unsigned long* serial) {
	XSettingsBuffer buffer;
	XSettingsSetting* setting = NULL;
	XSettingsList* settings_list = NULL;
	CARD32 manager_serial = 0;
	CARD32 n_entries;

	char local_byte_order = xsettings_byte_order();

	buffer.pos = buffer.data = data;
	buffer.len = len;

	// check byte order in selection
	if(!fetch_card8(&buffer, (CARD8*)&buffer.byte_order)) {
		puts("Can't get byte order");
		goto fail;
	}

	if(buffer.byte_order != MSBFirst && buffer.byte_order != LSBFirst) {
		puts("Invalid byte order");
		goto fail;
	}

	buffer.pos += 3;

	// encoded serial by manager
	if(!fetch_card32(&buffer, &manager_serial, local_byte_order)) {
		puts("Can't get serial");
		goto fail;
	}

	if(serial)
		*serial = manager_serial;

	if(!fetch_card32(&buffer, &n_entries, local_byte_order)) {
		puts("Can't get number of entries");
		return NULL;
	}

	for(unsigned int i = 0; i < n_entries; i++) {
		CARD8 type;
		CARD16 name_len;
		CARD32 v_int;
		/* size_t */ int pad_len;

		if(!fetch_card8(&buffer, &type)) {
			puts("Can't get type");
			goto fail;
		}

		buffer.pos += 1;

		if(!fetch_card16(&buffer, &name_len, local_byte_order)) {
			puts("Can't get name length");
			goto fail;
		}

		pad_len = XSETTINGS_PAD(name_len, 4);

		if(BYTES_LEFT(&buffer) < pad_len) {
			puts("Invalid settings size");
			goto fail;
		}

		setting = new XSettingsSetting;
		setting->type = XSETTINGS_TYPE_INT; /* no allocated memory ??? */

		setting->name = new char[name_len + 1];
		memcpy(setting->name, buffer.pos, name_len);
		setting->name[name_len] = '\0';
		buffer.pos += pad_len;

		if(!fetch_card32(&buffer, &v_int, local_byte_order)) {
			puts("Can't get serial");
			goto fail;
		}

		setting->last_change_serial = v_int;

		switch(type) {
			case XSETTINGS_TYPE_INT:
				if(!fetch_card32(&buffer, &v_int, local_byte_order)) {
					puts("Can't get int value");
					goto fail;
				}

				setting->data.v_int = v_int;
				break;
			case XSETTINGS_TYPE_STRING:
				if(!fetch_card32(&buffer, &v_int, local_byte_order)) {
					puts("Can't get string size");
					goto fail;
				}

				pad_len = XSETTINGS_PAD(v_int, 4);
				if(v_int + 1 == 0 || /* guard against wrap-around */
						BYTES_LEFT(&buffer) < pad_len) {
					puts("Wrong string size");
					goto fail;
				}

				setting->data.v_string = new char[v_int + 1];
				memcpy(setting->data.v_string, buffer.pos, v_int);
				setting->data.v_string[v_int] = '\0';
				buffer.pos += pad_len;
				break;
			case XSETTINGS_TYPE_COLOR:
				if(!fetch_ushort(&buffer, &setting->data.v_color.red, local_byte_order)) {
					puts("Can't get red color");
					goto fail;
				}

				if(!fetch_ushort(&buffer, &setting->data.v_color.green, local_byte_order)) {
					puts("Can't get green color");
					goto fail;
				}

				if(!fetch_ushort(&buffer, &setting->data.v_color.blue, local_byte_order)) {
					puts("Can't get blue color");
					goto fail;
				}

				if(!fetch_ushort(&buffer, &setting->data.v_color.alpha, local_byte_order)) {
					puts("Can't get alpha color");
					goto fail;
				}
				break;

			default:
				break;
		}

		setting->type = (XSettingsType)type;
		xsettings_list_add(&settings_list, setting);
		/* FIXME: free if failed */

		setting = NULL;
	}

	return settings_list;

fail:
	if(setting)
		xsettings_setting_free(setting);

	xsettings_list_free(settings_list);
	return NULL;
}

void xsettings_encode(const XSettingsSetting* setting, XSettingsBuffer* buffer) {
	int len, str_len;

	*(buffer->pos++) = setting->type;
	*(buffer->pos++) = 0;

	str_len = strlen(setting->name);
	*(CARD16*)(buffer->pos) = str_len;
	buffer->pos += 2;

	len = XSETTINGS_PAD(str_len, 4);

	memcpy(buffer->pos, setting->name, str_len);
	len -= str_len;
	buffer->pos += str_len;

	for(; len > 0; len--)
		*(buffer->pos++) = 0;

	*(CARD32*)(buffer->pos) = setting->last_change_serial;
	buffer->pos += 4;

	switch(setting->type) {
		case XSETTINGS_TYPE_INT:
			*(CARD32*)(buffer->pos) = setting->data.v_int;
			buffer->pos += 4;
			break;
		case XSETTINGS_TYPE_COLOR:
			*(CARD16*)(buffer->pos) = setting->data.v_color.red;
			*(CARD16*)(buffer->pos + 2) = setting->data.v_color.green;
			*(CARD16*)(buffer->pos + 4) = setting->data.v_color.blue;
			*(CARD16*)(buffer->pos + 6) = setting->data.v_color.alpha;
			buffer->pos += 8;
			break;
		case XSETTINGS_TYPE_STRING:
			str_len = strlen(setting->data.v_string);
			*(CARD32*)(buffer->pos) = str_len;
			buffer->pos += 4;

			len = XSETTINGS_PAD(str_len, 4);
			memcpy(buffer->pos, setting->data.v_string, str_len);
			len -= str_len;
			buffer->pos += str_len;

			for(; len > 0; len--)
				*(buffer->pos++) = 0;

			break;
	}
}

void xsettings_manager_notify(XSettingsData* data) {
	XSettingsBuffer buffer;
	XSettingsList* iter;
	int n_settings = 0;

	buffer.len = 12; /* byte-order + pad + SERIAL + N_SETTINGS */

	iter = data->settings;
	while(iter) {
		buffer.len += xsettings_setting_len(iter->setting);
		n_settings++;
		iter = iter->next;
	}

	buffer.data = new unsigned char[buffer.len];
	buffer.pos = buffer.data;

	*buffer.pos = xsettings_byte_order();

	buffer.pos += 4;
	*(CARD32*)buffer.pos = data->serial++;
	buffer.pos += 4;
	*(CARD32*)buffer.pos = n_settings;
	buffer.pos += 4;

	iter = data->settings;
	while(iter) {
		xsettings_encode(iter->setting, &buffer);
		iter = iter->next;
	}

	XChangeProperty(data->display, data->manager_win, data->xsettings_atom, data->xsettings_atom,
			8, PropModeReplace, buffer.data, buffer.len);

	delete [] buffer.data;
}

void xsettings_manager_set_setting(XSettingsData* data, XSettingsSetting* setting) {
	XSettingsSetting* old_setting = xsettings_list_find(data->settings, setting->name);

	if(old_setting) {
		if(xsettings_setting_equal(old_setting, setting))
			return;

		printf("removing %s\n", setting->name);
		xsettings_list_remove(&data->settings, setting->name);
	}

	XSettingsSetting* new_setting = xsettings_setting_copy(setting);
	new_setting->last_change_serial = data->serial;

	if(!xsettings_list_add(&data->settings, new_setting))
		xsettings_setting_free(new_setting);

	printf("adding %s\n", new_setting->name);
}

EDELIB_NS_END
