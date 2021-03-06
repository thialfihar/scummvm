/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "fullpipe/fullpipe.h"

#include "fullpipe/objectnames.h"
#include "fullpipe/constants.h"

#include "fullpipe/gameloader.h"
#include "fullpipe/motion.h"
#include "fullpipe/scenes.h"
#include "fullpipe/statics.h"

#include "fullpipe/interaction.h"
#include "fullpipe/behavior.h"


namespace Fullpipe {

struct WalkingBearder {
	StaticANIObject *ani;
	int wbflag;
	int wbcounter;
};

void scene29_initScene(Scene *sc) {
	g_vars->scene29_var01 = 300;
	g_vars->scene29_var02 = 200;
	g_vars->scene29_var03 = 400;
	g_vars->scene29_var04 = 300;
	g_vars->scene29_porter = sc->getStaticANIObject1ById(ANI_PORTER, -1);
	g_vars->scene29_shooter1 = sc->getStaticANIObject1ById(ANI_SHOOTER1, -1);
	g_vars->scene29_shooter2 = sc->getStaticANIObject1ById(ANI_SHOOTER2, -1);
	g_vars->scene29_ass = sc->getStaticANIObject1ById(ANI_ASS, -1);

	g_vars->scene29_var05.numBalls = 0;
	g_vars->scene29_var05.pTail = 0;
	g_vars->scene29_var05.field_8 = 0;
	g_vars->scene29_var05.pHead = 0;

	free(g_vars->scene29_var05.cPlex);
	g_vars->scene29_var05.cPlex = 0;

	StaticANIObject *ani;

	g_vars->scene29_var08.numBalls = 0;
	g_vars->scene29_var08.pTail = 0;
	g_vars->scene29_var08.field_8 = 0;
	g_vars->scene29_var08.pHead = 0;

	free(g_vars->scene29_var08.cPlex);
	g_vars->scene29_var08.cPlex = 0;

	ani = sc->getStaticANIObject1ById(ANI_SHELL_GREEN, -1);
	Ball *b = g_vars->scene29_var05.sub04(g_vars->scene29_var05.field_8, 0);
	b->ani = ani;

	if (g_vars->scene29_var05.field_8)
		g_vars->scene29_var05.field_8->p0 = b;
	else
		g_vars->scene29_var05.pHead = b;

	g_vars->scene29_var05.field_8 = b;

	for (int i = 0; i < 2; i++) {
		StaticANIObject *newani = new StaticANIObject(ani);

		sc->addStaticANIObject(newani, 1);

		b = g_vars->scene29_var05.sub04(g_vars->scene29_var05.field_8, 0);
		b->ani = ani;

		if (g_vars->scene29_var05.field_8)
			g_vars->scene29_var05.field_8->p0 = b;
		else
			g_vars->scene29_var05.pHead = b;

		g_vars->scene29_var05.field_8 = b;
	}

	g_vars->scene29_var06.numBalls = 0;
	g_vars->scene29_var06.pTail = 0;
	g_vars->scene29_var06.field_8 = 0;
	g_vars->scene29_var06.pHead = 0;

	free(g_vars->scene29_var06.cPlex);
	g_vars->scene29_var06.cPlex = 0;

	g_vars->scene29_var07.numBalls = 0;
	g_vars->scene29_var07.pTail = 0;
	g_vars->scene29_var07.field_8 = 0;
	g_vars->scene29_var07.pHead = 0;

	free(g_vars->scene29_var07.cPlex);
	g_vars->scene29_var07.cPlex = 0;

	ani = sc->getStaticANIObject1ById(ANI_SHELL_RED, -1);

	b = g_vars->scene29_var06.sub04(g_vars->scene29_var06.field_8, 0);
	b->ani = ani;

	if (g_vars->scene29_var06.field_8)
		g_vars->scene29_var06.field_8->p0 = b;
	else
		g_vars->scene29_var06.pHead = b;

	g_vars->scene29_var06.field_8 = b;

	for (int i = 0; i < 2; i++) {
		StaticANIObject *newani = new StaticANIObject(ani);

		sc->addStaticANIObject(newani, 1);

		b = g_vars->scene29_var06.sub04(g_vars->scene29_var06.field_8, 0);
		b->ani = ani;

		if (g_vars->scene29_var06.field_8)
			g_vars->scene29_var06.field_8->p0 = b;
		else
			g_vars->scene29_var06.pHead = b;

		g_vars->scene29_var06.field_8 = b;
	}

	g_vars->scene29_var19.clear();

	ani = new StaticANIObject(g_fp->accessScene(SC_COMMON)->getStaticANIObject1ById(ANI_BEARDED_CMN, -1));

	ani->_statics = ani->getStaticsById(ST_BRDCMN_EMPTY);

	sc->addStaticANIObject(ani, 1);

	WalkingBearder *wb = new WalkingBearder;

	wb->ani = ani;
	wb->wbflag = 0;
	wb->wbcounter = 0;

	g_vars->scene29_var19.push_back(wb);

	g_vars->scene29_var09 = 0;
	g_vars->scene29_var10 = 0;
	g_vars->scene29_var11 = 0;
	g_vars->scene29_var12 = 0;
	g_vars->scene29_var13 = 0;
	g_vars->scene29_var14 = 75;
	g_vars->scene29_var15 = 0;
	g_vars->scene29_var16 = 0;
	g_vars->scene29_var17 = 0;
	g_vars->scene29_var18 = 0;

	g_fp->setArcadeOverlay(PIC_CSR_ARCADE8);
}

void sceneHandler29_winArcade() {
	warning("STUB: sceneHandler29_winArcade()");
}

void sceneHandler29_shootGreen() {
	warning("STUB: sceneHandler29_shootGreen()");
}

void sceneHandler29_shootRed() {
	warning("STUB: sceneHandler29_shootRed()");
}

void sceneHandler29_manJump() {
	if (!g_fp->_aniMan->_movement || g_fp->_aniMan->_movement->_id == MV_MAN29_RUN || g_fp->_aniMan->_movement->_id == MV_MAN29_STANDUP) {
		g_vars->scene29_var12 = 0;
		g_vars->scene29_var15 = 0;
		g_vars->scene29_var11 = 1;

		g_fp->_aniMan->changeStatics2(ST_MAN29_RUNR);
		g_fp->_aniMan->startAnim(MV_MAN29_JUMP, 0, -1);
	}

	g_vars->scene29_var20 = g_fp->_aniMan->_ox;
	g_vars->scene29_var21 = g_fp->_aniMan->_oy;
}

void sceneHandler29_manBend() {
	if (!g_fp->_aniMan->_movement || g_fp->_aniMan->_movement->_id == MV_MAN29_RUN || g_fp->_aniMan->_movement->_id == MV_MAN29_STANDUP) {
		g_vars->scene29_var12 = 0;
		g_vars->scene29_var15 = 0;
		g_vars->scene29_var11 = 1;

		g_fp->_aniMan->changeStatics2(ST_MAN29_RUNR);
		g_fp->_aniMan->startAnim(MV_MAN29_BEND, 0, -1);
	}

	g_vars->scene29_var20 = g_fp->_aniMan->_ox;
	g_vars->scene29_var21 = g_fp->_aniMan->_oy;
}

void sceneHandler29_sub03() {
	warning("STUB: sceneHandler29_sub03()");
}

void sceneHandler29_manFromL() {
	if (g_vars->scene29_var20 < 497 && !g_vars->scene29_var17) {
		getCurrSceneSc2MotionController()->setEnabled();
		getGameLoaderInteractionController()->enableFlag24();

		g_fp->_aniMan->changeStatics2(ST_MAN_RIGHT | 0x4000);
		chainQueue(QU_SC29_MANFROM_L, 1);

		g_vars->scene29_var17 = 1;

		g_fp->_scrollSpeed = g_vars->scene29_var16;
	}
}

void sceneHandler29_manFromR() {
	getCurrSceneSc2MotionController()->setEnabled();
	getGameLoaderInteractionController()->enableFlag24();

	chainQueue(QU_SC29_MANFROM_R, 1);

	g_vars->scene29_var10 = 0;
	g_vars->scene29_var12 = 0;
}

int sceneHandler29_updateScreenCallback() {
	int res;

	res = g_fp->drawArcadeOverlay(g_vars->scene29_var10);

	if (!res)
		g_fp->_updateScreenCallback = 0;

	return res;
}

void sceneHandler29_manToL() {
	getCurrSceneSc2MotionController()->clearEnabled();
	getGameLoaderInteractionController()->disableFlag24();

	chainQueue(QU_SC29_MANTO_L, 1);

	g_vars->scene29_var10 = 1;

	g_vars->scene29_mgm.addItem(g_fp->_aniMan->_id);

	g_fp->_updateScreenCallback = sceneHandler29_updateScreenCallback;

	g_fp->_msgY = -1;
	g_fp->_msgX = -1;
}

void sceneHandler29_manToR() {
	getCurrSceneSc2MotionController()->clearEnabled();
	getGameLoaderInteractionController()->disableFlag24();

	chainQueue(QU_SC29_MANTO_R, 1);

	g_vars->scene29_var09 = 1;
	g_fp->_msgY = -1;
	g_fp->_msgX = -1;

	g_vars->scene29_var17 = 0;

	g_vars->scene29_var16 = g_fp->_scrollSpeed;
	g_fp->_scrollSpeed = 4;
}

void sceneHandler29_clickPorter(ExCommand *cmd) {
	if (!g_fp->_aniMan->isIdle() || g_fp->_aniMan->_flags & 0x100) {
		cmd->_messageKind = 0;

		return;
	}

	if (g_vars->scene29_var20 <= g_vars->scene29_porter->_ox) {
		if (ABS(351 - g_vars->scene29_var20) > 1 || ABS(443 - g_vars->scene29_var21) > 1
			|| g_fp->_aniMan->_movement || g_fp->_aniMan->_statics->_staticsId != ST_MAN_RIGHT) {
			if (g_fp->_msgX != 351 || g_fp->_msgY != 443) {
				MessageQueue *mq = getCurrSceneSc2MotionController()->method34(g_fp->_aniMan, 351, 443, 1, ST_MAN_RIGHT);

				if (mq) {
					mq->addExCommandToEnd(cmd->createClone());

					postExCommand(g_fp->_aniMan->_id, 2, 351, 443, 0, -1);
				}
			}
		} else {
			sceneHandler29_manToL();
		}
	} else {
		g_vars->scene29_var20 = g_fp->_aniMan->_ox;
		g_vars->scene29_var21 = g_fp->_aniMan->_oy;

		if (ABS(1582 - g_vars->scene29_var20) > 1 || ABS(445 - g_fp->_aniMan->_oy) > 1
			|| g_fp->_aniMan->_movement || g_fp->_aniMan->_statics->_staticsId != (0x4000 | ST_MAN_RIGHT)) {
			if (g_fp->_msgX != 1582 || g_fp->_msgY != 445) {
				MessageQueue *mq = getCurrSceneSc2MotionController()->method34(g_fp->_aniMan, 1582, 445, 1, (0x4000 | ST_MAN_RIGHT));

				if (mq) {
					mq->addExCommandToEnd(cmd->createClone());

					postExCommand(g_fp->_aniMan->_id, 2, 1582, 445, 0, -1);
				}
			}
		} else {
			sceneHandler29_manToR();
		}
	}
}

void sceneHandler29_sub05() {
	warning("STUB: sceneHandler29_sub05()");
}

void sceneHandler29_shootersEscape() {
	if (g_vars->scene29_var10) {
		g_vars->scene29_var20 += 2;

		g_fp->_aniMan->setOXY(g_vars->scene29_var20, g_vars->scene29_var21);

		if (g_vars->scene29_var20 > 1310 && !g_vars->scene29_shooter1->_movement && !g_vars->scene29_shooter2->_movement
			&& g_vars->scene29_shooter1->_statics->_staticsId == ST_STR1_RIGHT) {
			g_vars->scene29_var13 = 0;

			g_vars->scene29_shooter1->changeStatics2(ST_STR1_STAND);
			g_vars->scene29_shooter2->changeStatics2(ST_STR2_STAND);

			chainQueue(QU_SC29_ESCAPE, 1);

			g_vars->scene29_ass->queueMessageQueue(0);
			g_vars->scene29_ass->hide();

			g_fp->setObjectState(sO_LeftPipe_29, g_fp->getObjectEnumState(sO_LeftPipe_29, sO_IsOpened));
		}
	} else if (g_vars->scene29_var09) {
		g_vars->scene29_var20 -= 4;

		g_fp->_aniMan->setOXY(g_vars->scene29_var20, g_vars->scene29_var21);
	}
}

void sceneHandler29_sub07() {
	warning("STUB: sceneHandler29_sub07()");
}

void sceneHandler29_assHitGreen() {
	if (g_vars->scene29_ass->_statics->_staticsId == ST_ASS_NORM) {
		g_vars->scene29_ass->changeStatics2(ST_ASS_NORM);
		g_vars->scene29_ass->startAnim(MV_ASS_HITGREEN, 0, -1);
	}
}

void sceneHandler29_assHitRed() {
	if (g_vars->scene29_ass->_statics->_staticsId == ST_ASS_NORM) {
		g_vars->scene29_ass->changeStatics2(ST_ASS_NORM);
		g_vars->scene29_ass->startAnim(MV_ASS_HITRED, 0, -1);
	}
}

void sceneHandler29_shoot() {
	if (g_vars->scene29_var10 && g_vars->scene29_var20 < 1310) {
		if (g_fp->_rnd->getRandomNumber(1) || g_vars->scene29_shooter1->_movement || g_vars->scene29_shooter1->_statics->_staticsId != ST_STR1_RIGHT) {
			if (!g_vars->scene29_shooter2->_movement && g_vars->scene29_shooter2->_statics->_staticsId == ST_STR2_RIGHT) {
				if (g_vars->scene29_shooter2->_flags & 4) {
					g_vars->scene29_shooter2->startAnim(MV_STR2_SHOOT, 0, -1);

					g_vars->scene29_var13 = 0;
				}
			}
		} else {
			g_vars->scene29_shooter1->startAnim(MV_STR1_SHOOT, 0, -1);

			g_vars->scene29_var13 = 0;
		}
	}
}

void sceneHandler29_animBearded() {
	warning("STUB: sceneHandler29_animBearded()");
}



int sceneHandler29(ExCommand *cmd) {
	if (cmd->_messageKind != 17)
		return 0;

	switch (cmd->_messageNum) {
	case MSG_CMN_WINARCADE:
		sceneHandler29_winArcade();
		break;

	case MSG_SC29_LAUGH:
		if (g_vars->scene29_var18 == ANI_SHELL_GREEN) {
			g_fp->playSound(SND_29_028, 0);
			break;
		}

		g_fp->playSound(SND_29_029, 0);

		break;

	case MSG_SC29_SHOWLASTRED:
		if (g_vars->scene29_var05.numBalls) {
			g_vars->scene29_var06.field_8->ani->show1(-1, -1, -1, 0);
			g_vars->scene29_var06.field_8->ani->startAnim(MV_SHR_HITASS, 0, -1);
		}

		break;

	case MSG_SC29_SHOOTGREEN:
		sceneHandler29_shootGreen();
		break;

	case MSG_SC29_SHOOTRED:
		sceneHandler29_shootRed();
		break;

	case MSG_SC29_SHOWLASTGREEN:
		if (g_vars->scene29_var05.numBalls) {
			g_vars->scene29_var05.field_8->ani->show1(-1, -1, -1, 0);
			g_vars->scene29_var05.field_8->ani->startAnim(MV_SHG_HITASS, 0, -1);
		}

		break;

	case MSG_SC29_STOPRIDE:
		g_vars->scene29_var09 = 0;
		g_vars->scene29_var10 = 0;
		g_vars->scene29_var11 = 0;
		g_vars->scene29_var12 = 0;

		getCurrSceneSc2MotionController()->setEnabled();
		getGameLoaderInteractionController()->enableFlag24();
		break;

	case MSG_SC29_DISABLERIDEBACK:
		g_vars->scene29_var12 = 0;
		break;

	case MSG_SC29_ENABLERIDEBACK:
		g_vars->scene29_var12 = 1;
		g_vars->scene29_var11 = 0;
		break;

	case MSG_SC29_DISABLEPORTER:
		g_vars->scene29_var11 = 0;
		break;

	case MSG_SC29_ENABLEPORTER:
		g_vars->scene29_var11 = 1;
		g_vars->scene29_var12 = 0;
		g_vars->scene29_var15 = 0;
		break;

	case 29:
		if (!g_vars->scene29_var09 || g_vars->scene29_var10) {
			if (!g_vars->scene29_var10) {
				StaticANIObject *ani = g_fp->_currentScene->getStaticANIObjectAtPos(g_fp->_sceneRect.left + cmd->_x, g_fp->_sceneRect.top + cmd->_y);

				if (ani && ani == g_vars->scene29_porter) {
					sceneHandler29_clickPorter(cmd);

					cmd->_messageKind = 0;
					break;
				}
				break;
			}

			sceneHandler29_manJump();

			cmd->_messageKind = 0;
			break;
		}
		break;

	case 107:
		if (g_vars->scene29_var10)
			sceneHandler29_manBend();

		break;

	case 33:
		if (g_vars->scene29_var10) {
			if (g_vars->scene29_var20 > g_fp->_sceneRect.right - 500)
				g_fp->_currentScene->_x = g_fp->_sceneRect.right - g_vars->scene29_var20 - 350;

			if (g_vars->scene29_var20 < g_fp->_sceneRect.left + 100)
				g_fp->_currentScene->_x = g_vars->scene29_var20 - g_fp->_sceneRect.left - 100;

		} else if (g_fp->_aniMan2) {
			int x = g_fp->_aniMan2->_ox;

			if (x < g_fp->_sceneRect.left + g_vars->scene29_var01)
				g_fp->_currentScene->_x = x - g_vars->scene29_var03 - g_fp->_sceneRect.left;

			if (x > g_fp->_sceneRect.right - g_vars->scene29_var01)
				g_fp->_currentScene->_x = x + g_vars->scene29_var03 - g_fp->_sceneRect.right;
		}

		g_vars->scene29_var20 = g_fp->_aniMan->_ox;
		g_vars->scene29_var21 = g_fp->_aniMan->_oy;

		sceneHandler29_sub03();

		if (!g_vars->scene29_porter->_movement)
			g_vars->scene29_porter->startAnim(MV_PTR_MOVEFAST, 0, -1);

		if (g_vars->scene29_var09)
			sceneHandler29_manFromL();
		else if (g_vars->scene29_var10 && !g_fp->_aniMan->_movement)
			sceneHandler29_sub05();

		if (g_vars->scene29_var11)
			sceneHandler29_shootersEscape();
		else if (g_vars->scene29_var12)
			sceneHandler29_sub07();

		g_vars->scene29_var13++;

		if (g_vars->scene29_var13 > g_vars->scene29_var14)
			sceneHandler29_shoot();

		sceneHandler29_animBearded();

		g_fp->_behaviorManager->updateBehaviors();
		g_fp->startSceneTrack();

		break;
	}

	return 0;
}

int scene29_updateCursor() {
	g_fp->updateCursorCommon();

	if (g_vars->scene29_var10) {
		if (g_fp->_cursorId != PIC_CSR_DEFAULT_INV && g_fp->_cursorId != PIC_CSR_ITN_INV)
			g_fp->_cursorId = -1;
	} else if (g_vars->scene29_var09) {
		if (g_fp->_cursorId != PIC_CSR_DEFAULT_INV && g_fp->_cursorId != PIC_CSR_ITN_INV)
			g_fp->_cursorId = PIC_CSR_DEFAULT;
	} else if (g_fp->_objectIdAtCursor == ANI_PORTER) {
		if (g_fp->_cursorId == PIC_CSR_DEFAULT)
			g_fp->_cursorId = PIC_CSR_ITN;
	} else {
		if (g_fp->_objectIdAtCursor == PIC_SC29_LTRUBA && g_fp->_cursorId == PIC_CSR_ITN)
			g_fp->_cursorId = PIC_CSR_GOL;
	}
	return g_fp->_cursorId;
}

} // End of namespace Fullpipe
