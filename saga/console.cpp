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

// Console module

#include "saga/saga.h"
#include "saga/gfx.h"
#include "saga/font.h"
#include "saga/cvar_mod.h"
#include "saga/events.h"

#include "saga/console.h"

namespace Saga {

int Console::reg() {
	CVAR_Register_I(&_resize, "con_h", NULL, CVAR_NONE, 12, CON_DEFAULTPOS);
	CVAR_Register_I(&_droptime, "con_droptime", NULL, CVAR_NONE, 0, 5000);
	CVAR_Register_I(&_lineMax, "con_lines", NULL, CVAR_NONE, 5, 5000);
	return SUCCESS;
}

Console::Console(SagaEngine *vm) : _vm(vm) {
	memset(&_scrollback, 0, sizeof(_scrollback));
	memset(&_history, 0, sizeof(_history));

	_resize = CON_DEFAULTPOS;
	_droptime = CON_DROPTIME;

	_active = false;
	_yMax = CON_DEFAULTPOS;
	_lineMax = CON_DEFAULTLINES;
	_histMax = CON_DEFAULTCMDS;
	_histPos = 0;
	_linePos = 0;
	_yPos = 0;
	_prompt = NULL;
	_promptW = 0;
	*_inputBuf = 0;
	_inputPos = 0;
}

Console::~Console() {
	debug(0, "~Console(): Deleting console scrollback and command history.");

	deleteScroll(&_scrollback);
	deleteScroll(&_history);
}

int Console::activate() {
	EVENT con_event;

	if (_active) {
		return FAILURE;
	}

	con_event.type = CONTINUOUS_EVENT;
	con_event.code = CONSOLE_EVENT | NODESTROY;
	con_event.op = EVENT_ACTIVATE;
	con_event.time = 0;
	con_event.duration = _droptime;

	_vm->_events->queue(&con_event);

	_active = true;

	return SUCCESS;
}

int Console::deactivate() {
	EVENT con_event;

	if (!_active) {
		return FAILURE;
	}

	con_event.type = CONTINUOUS_EVENT;
	con_event.code = CONSOLE_EVENT | NODESTROY;
	con_event.op = EVENT_DEACTIVATE;
	con_event.time = 0;
	con_event.duration = _droptime;

	_vm->_events->queue(&con_event);

	return SUCCESS;
}

bool Console::isActive(void) {
	return _active;
}

// Responsible for processing character input to the console and maintaining
// the console input buffer.
// Input buffer is processed by EXPR_Parse on enter.
// High ASCII characters are ignored.
int Console::type(int in_char) {
	int input_pos = _inputPos;
	const char *expr;
	int expr_len;
	int result;
	//char *lvalue;

	char *rvalue = NULL;
	CVAR_P con_cvar = NULL;

	const char *expr_err;
	const char *err_str;

	if (_yPos != _yMax) {
		// Ignore keypress until console fully down
		return SUCCESS;
	}

	if ((in_char > 127) || (!in_char)) {
		// Ignore non-ascii codes
		return SUCCESS;
	}

	switch (in_char) {
	case '\r':
		expr = _inputBuf;
		_vm->_console->print("> %s", _inputBuf);
		expr_len = strlen(_inputBuf);
		result = EXPR_Parse(&expr, &expr_len, &con_cvar, &rvalue);
		_vm->_console->addLine(&_history, _histMax, _inputBuf);
		memset(_inputBuf, 0, CON_INPUTBUF_LEN);
		_inputPos = 0;
		_histPos = 0;
		if (result != SUCCESS) {
			EXPR_GetError(&expr_err);
			_vm->_console->print("Parse error: %s", expr_err);
			break;
		}

		if (rvalue == NULL) {
			CVAR_Print(con_cvar);
			break;
		}

		if (CVAR_IsFunc(con_cvar)) {
			CVAR_Exec(con_cvar, rvalue);
		} else if (CVAR_SetValue(con_cvar, rvalue) != SUCCESS) {
			CVAR_GetError(&err_str);
			_vm->_console->print("Illegal assignment: %s.", err_str);
		}
		break;
	case '\b':
		_inputBuf[input_pos] = 0;
		if (input_pos > 0) {
			_inputPos--;
			_inputBuf[_inputPos] = 0;
		}
		break;
	default:
		if (input_pos < CON_INPUTBUF_LEN) {
			_inputBuf[input_pos] = (char)in_char;
			_inputPos++;
		}
		break;
	}

	if (rvalue)
		free(rvalue);

	return SUCCESS;
}

int Console::draw(SURFACE *ds) {
	int line_y;
	CONSOLE_LINE *walk_ptr;
	CONSOLE_LINE *start_ptr;
	int txt_fgcolor;
	int txt_shcolor;
	Rect fill_rect;
	int i;

	if (!_active) {
		return FAILURE;
	}

	if (_resize != _yMax) {
		_yMax = _resize;
		_yPos = _resize;
	}

	fill_rect.top = 0;
	fill_rect.left = 0;
	fill_rect.bottom = _yPos + 1;
	fill_rect.right = ds->w;

	drawRect(ds, &fill_rect, _vm->_gfx->matchColor(CONSOLE_BGCOLOR));
	txt_fgcolor = _vm->_gfx->matchColor(CONSOLE_TXTCOLOR);
	txt_shcolor = _vm->_gfx->matchColor(CONSOLE_TXTSHADOW);

	_vm->_font->draw(SMALL_FONT_ID, ds, ">", 1, 2, _yPos - 10, txt_fgcolor, txt_shcolor, FONT_SHADOW);
	_vm->_font->draw(SMALL_FONT_ID, ds, _inputBuf, strlen(_inputBuf),
				10, _yPos - 10, txt_fgcolor, txt_shcolor, FONT_SHADOW);

	line_y = _yPos - (CON_INPUT_H + CON_LINE_H);
	start_ptr = _scrollback.head;

	for (i = 0; i < _linePos; i++) {
		if (start_ptr->next) {
			start_ptr = start_ptr->next;
		} else {
			break;
		}
	}

	for (walk_ptr = start_ptr; walk_ptr; walk_ptr = walk_ptr->next) {
		_vm->_font->draw(SMALL_FONT_ID, ds, walk_ptr->str_p, walk_ptr->str_len, 2, line_y, txt_fgcolor, txt_shcolor, FONT_SHADOW);
		line_y -= CON_LINE_H;
		if (line_y < -CON_LINE_H)
			break;
	}

	return SUCCESS;
}

int Console::print(const char *fmt_str, ...) {
	char vsstr_p[CON_PRINTFLIMIT + 1];
	va_list argptr;
	int ret_val;

	va_start(argptr, fmt_str);
	ret_val = vsprintf(vsstr_p, fmt_str, argptr);
	_vm->_console->addLine(&_scrollback, _lineMax, vsstr_p);
	debug(0, vsstr_p);
	va_end(argptr);
	_linePos = 0;

	return ret_val;
}

int Console::cmdUp() {
	CONSOLE_LINE *start_ptr = _history.head;
	int i;

	if (!start_ptr) {
		return SUCCESS;
	}

	if (_histPos < _history.lines) {
		_histPos++;
	}

	for (i = 1; (i < _histPos); i++) {
		if (start_ptr->next) {
			start_ptr = start_ptr->next;
		} else {
			break;
		}
	}

	memset(_inputBuf, 0, CON_INPUTBUF_LEN);
	strcpy(_inputBuf, start_ptr->str_p);
	_inputPos = start_ptr->str_len - 1;

	debug(0, "History pos: %d/%d", _histPos, _history.lines);

	return SUCCESS;
}

int Console::cmdDown(void) {
	CONSOLE_LINE *start_ptr = _history.head;
	int i;

	if (_histPos == 1) {
		debug(0, "Erased input buffer.");
		memset(_inputBuf, 0, CON_INPUTBUF_LEN);
		_inputPos = 0;
		_histPos--;
		return SUCCESS;
	} else if (_histPos) {
		_histPos--;
	} else {
		return SUCCESS;
	}

	for (i = 1; i < _histPos; i++) {
		if (start_ptr->next) {
			start_ptr = start_ptr->next;
		} else {
			break;
		}
	}

	memset(_inputBuf, 0, CON_INPUTBUF_LEN);
	strcpy(_inputBuf, start_ptr->str_p);
	_inputPos = start_ptr->str_len - 1;

	debug(0, "History pos: %d/%d", _histPos, _history.lines);

	return SUCCESS;
}

int Console::pageUp() {
	int n_lines;
	n_lines = (_yMax - CON_INPUT_H) / CON_LINE_H;

	if (_linePos < (_scrollback.lines - n_lines)) {
		_linePos += n_lines;
	}

	debug(0, "Line pos: %d", _linePos);
	return SUCCESS;
}

int Console::pageDown() {
	int n_lines;
	n_lines = (_yMax - CON_INPUT_H) / CON_LINE_H;

	if (_linePos > n_lines) {
		_linePos -= n_lines;
	} else {
		_linePos = 0;
	}

	return SUCCESS;
}

int Console::dropConsole(double percent) {
	SURFACE *back_buf;

	if (percent > 1.0) {
		percent = 1.0;
	}

	back_buf = _vm->_gfx->getBackBuffer();
	_vm->_console->setDropPos(percent);
	_vm->_console->draw(back_buf);

	return SUCCESS;
}

int Console::raiseConsole(double percent) {
	SURFACE *back_buf;

	if (percent >= 1.0) {
		percent = 1.0;
		_active = false;
	}

	back_buf = _vm->_gfx->getBackBuffer();
	_vm->_console->setDropPos(1.0 - percent);
	_vm->_console->draw(back_buf);

	return SUCCESS;
}

int Console::setDropPos(double percent) {
	double exp_percent;

	if (percent > 1.0)
		percent = 1.0;
	if (percent < 0.0)
		percent = 0.0;

	exp_percent = percent * percent;
	_yPos = (int)(_yMax * exp_percent);

	return SUCCESS;
}

int Console::addLine(CON_SCROLLBACK *scroll, int line_max, const char *constr_p) {
	int constr_len;
	char *newstr_p;
	CONSOLE_LINE *newline_p;
	int del_lines;
	int i;

	constr_len = strlen(constr_p) + 1;
	newstr_p = (char *)malloc(constr_len);
	if (newstr_p == NULL) {
		return MEM;
	}

	newline_p = (CONSOLE_LINE *)malloc(sizeof(CONSOLE_LINE));
	if (newline_p == NULL) {
		return MEM;
	}
	newline_p->next = NULL;
	newline_p->prev = NULL;

	strcpy(newstr_p, constr_p);
	newline_p->str_p = newstr_p;
	newline_p->str_len = constr_len;

	if (scroll->head == NULL) {
		scroll->head = newline_p;
		scroll->tail = newline_p;
	} else {
		scroll->head->prev = newline_p;
		newline_p->next = scroll->head;
		scroll->head = newline_p;
	}

	scroll->lines++;

	if (scroll->lines > line_max) {
		del_lines = scroll->lines - line_max;

		for (i = 0; i < del_lines; i++) {
			_vm->_console->deleteLine(scroll);
		}
	}

	return SUCCESS;
}

int Console::deleteLine(CON_SCROLLBACK *scroll) {
	CONSOLE_LINE *temp_p = scroll->tail;

	if (temp_p->prev == NULL) {
		scroll->head = NULL;
		scroll->tail = NULL;
	} else {
		temp_p->prev->next = NULL;
		scroll->tail = temp_p->prev;
	}

	if (temp_p->str_p)
		free(temp_p->str_p);
	free(temp_p);
	scroll->lines--;

	return SUCCESS;
}

int Console::deleteScroll(CON_SCROLLBACK * scroll) {
	CONSOLE_LINE *walk_ptr;
	CONSOLE_LINE *temp_ptr;

	for (walk_ptr = scroll->head; walk_ptr; walk_ptr = temp_ptr) {

		if (walk_ptr->str_p)
			free(walk_ptr->str_p);
		temp_ptr = walk_ptr->next;
		free(walk_ptr);
	}

	return SUCCESS;
}

} // End of namespace Saga
