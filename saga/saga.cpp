/* ScummVM - Scumm Interpreter
 * Copyright (C) 2004 The ScummVM project
 *
 * The ReInherit Engine is (C)2000-2003 by Daniel Balsom.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */
#include "stdafx.h"

#include "base/gameDetector.h"
#include "base/plugins.h"
#include "backends/fs/fs.h"

#include "sound/mixer.h"
#include "common/file.h"
#include "common/config-manager.h"

#include "saga/saga.h"

#include "saga/gfx.h"
#include "saga/rscfile_mod.h"
#include "saga/render.h"
#include "saga/actor.h"
#include "saga/animation.h"
#include "saga/console_mod.h"
#include "saga/cvar_mod.h"
#include "saga/events_mod.h"
#include "saga/actionmap.h"
#include "saga/font.h"
#include "saga/game_mod.h"
#include "saga/game.h"
#include "saga/interface_mod.h"
#include "saga/isomap.h"
#include "saga/script.h"
#include "saga/script_mod.h"
#include "saga/scene_mod.h"
#include "saga/sdata.h"
#include "saga/sndres.h"
#include "saga/sprite_mod.h"
#include "saga/objectmap.h"
#include "saga/sound.h"
#include "saga/music.h"
#include "saga/game_mod.h"

GameList Engine_SAGA_gameList() {
	return Saga::GAME_GameList();
}

DetectedGameList Engine_SAGA_detectGames(const FSList &fslist) {
	return Saga::GAME_ProbeGame(fslist);
}

Engine *Engine_SAGA_create(GameDetector *detector, OSystem *syst) {
	return new Saga::SagaEngine(detector, syst);
}

REGISTER_PLUGIN("SAGA Engine", Engine_SAGA_gameList, Engine_SAGA_create, Engine_SAGA_detectGames)

namespace Saga {

#define R_MAX_TIME_DELTA 100

struct R_MAIN_DATA {
	int sound_enabled;
	int music_enabled;
};

static void CF_quitfunc(int argc, char *argv[], void *refCon);
static void CF_testfunc(int argc, char *argv[], void *refCon);

static R_MAIN_DATA MainData;

SagaEngine *_vm = NULL;

SagaEngine::SagaEngine(GameDetector *detector, OSystem *syst)
	: Engine(syst) {

	GAME_setGameDirectory(getGameDataPath());

	// Setup mixer
	if (!_mixer->isReady()) {
		warning("Sound initialization failed.");
	}

	_mixer->setVolume(ConfMan.getInt("sfx_volume") * ConfMan.getInt("master_volume") / 255);

	_vm = this;
}

SagaEngine::~SagaEngine() {
}

void SagaEngine::errorString(const char *buf1, char *buf2) {
	strcpy(buf2, buf1);
}

void SagaEngine::go() {
	int msec = 0;

	// Register engine modules
	CON_Register(); // Register console cvars first

	GAME_Register();
	SCENE_Register();

	MainData.sound_enabled = 1;
	MainData.music_enabled = 1;

	CVAR_RegisterFunc(CF_testfunc, "testfunc", "foo [ optional foo ]", R_CVAR_NONE, 0, -1, this);

	CVAR_Register_I(&MainData.sound_enabled, "sound", NULL, R_CVAR_CFG, 0, 1);

	CVAR_Register_I(&MainData.music_enabled, "music", NULL, R_CVAR_CFG, 0, 1);

	CVAR_RegisterFunc(CF_quitfunc, "quit", NULL, R_CVAR_NONE, 0, 0, this);

	// Process config file
	// FIXME
	/*
	if (CFG_Read(NULL) != R_SUCCESS) {
		warning("Couldn't read configuration file");
	}
	*/

	// Process command line

	// Detect game and open resource files
	if (GAME_Init() != R_SUCCESS) {
		warning("Couldn't start the game: %s", GAME_GetErrS());
		return;
	}

	// Initialize engine modules
	_sndRes = new SndRes(this);
	EVENT_Init();
	_font = new Font(this);
	SPRITE_Init();
	_anim = new Anim(this);
	_script = new Script();
	_sdata = new SData();
	INTERFACE_Init(); // requires script module
	_actor = new Actor(this);

	if (SCENE_Init() != R_SUCCESS) {
		warning("Couldn't initialize scene module");
		return;
	}

	// System initialization

	_previousTicks = _system->get_msecs();

	// On some platforms, graphics initialization also initializes sound
	// ( Win32 DirectX )... Music must be initialized before sound for 
	// native midi support
	MidiDriver *driver = GameDetector::createMidi(GameDetector::detectMusicDriver(MDT_NATIVE | MDT_ADLIB | MDT_PREFER_NATIVE));
	if (!driver)
		driver = MidiDriver_ADLIB_create(_mixer);
	else if (ConfMan.getBool("native_mt32"))
		driver->property(MidiDriver::PROP_CHANNEL_MASK, 0x03FE);

	_music = new Music(driver, MainData.music_enabled);
	_music->hasNativeMT32(ConfMan.getBool("native_mt32"));

	if (!MainData.music_enabled) {
		debug(0, "Music disabled.");
	}

	// Initialize graphics
	R_GAME_DISPLAYINFO disp_info;
	GAME_GetDisplayInfo(&disp_info);
	_gfx = new Gfx(_system, disp_info.logical_w, disp_info.logical_h);

	_isoMap = new IsoMap(_gfx);
	_actionMap = new ActionMap(this);
	_objectMap = new ObjectMap(_gfx);
	
	_render = new Render(this, _system, _gfx, _objectMap);
	if (!_render->initialized()) {
		return;
	}

	// Initialize system specific sound
	_sound = new Sound(this, _mixer, MainData.sound_enabled);
	if (!MainData.sound_enabled) {
		debug(0, "Sound disabled.");
	}

	_actor->reg();
	_script->reg();
	_render->reg();
	_anim->reg();
	_actionMap->reg();
	_objectMap->reg();

	_previousTicks = _system->get_msecs();

	// Begin Main Engine Loop

	SCENE_Start();
	uint32 currentTicks;

	for (;;) {
		if (_render->getFlags() & RF_RENDERPAUSE) {
			// Freeze time while paused
			_previousTicks = _system->get_msecs();
		} else {
			currentTicks = _system->get_msecs();
			// Timer has rolled over after 49 days
			if (currentTicks < _previousTicks)
				msec = 0;
			else {
				msec = currentTicks - _previousTicks;
				_previousTicks = currentTicks;
			}
			if (msec > R_MAX_TIME_DELTA) {
				msec = R_MAX_TIME_DELTA;
			}
			_actor->direct(msec);
			EVENT_HandleEvents(msec);
			STHREAD_ExecThreads(msec);
		}
		// Per frame processing
		_render->drawScene();
		_system->delay_msecs(0);
	}
}

void SagaEngine::shutdown() {
	SCENE_Shutdown();
	delete _actor;
	delete _script;
	SPRITE_Shutdown();
	delete _font;
	CON_Shutdown();
	CVAR_Shutdown();
	EVENT_Shutdown();

	delete _render;
	delete _actionMap;
	delete _isoMap;
	delete _objectMap;
	delete _sndRes;
	delete _sdata;
	// Shutdown system modules */
	delete _music;
	delete _sound;
	delete _anim;

	_system->quit();
}

static void CF_quitfunc(int argc, char *argv[], void *refCon) {
	((SagaEngine *)refCon)->shutdown();
	exit(0);
}

static void CF_testfunc(int argc, char *argv[], void *refCon) {
	int i;

	CON_Print("Test function invoked: Got %d arguments.", argc);

	for (i = 0; i < argc; i++) {
		CON_Print("Arg %d: %s", i, argv[i]);
	}
}

} // End of namespace Saga
