#include "Text_editor.h"


EditorView::EditorView(EditorModel* model, EditorController* controller, Target* adapter)
{
	_controller = controller;
	_adapter = adapter;
	model->AddObserver(this);
	_x = 0, _y = 0;
	_x_command = 0, _y_command = 0;
	_index_end_page = 0, _index_start_page = 0;
	_index_in_row = 0;

	_adapter->ApapterInitscr();
	_adapter->ApapterStartColor();
	_adapter->AdapterResizeTerm(cScreenHeight, cScreenWidth);

	_win_for_text = _adapter->AdapterNewWin(cTextScreenHeight, cScreenWidth, 0, 0);
	_win_for_command = _adapter->AdapterNewWin(cStatusBarHeight, cCommandWidth, cTextScreenHeight, 0);
	_win_for_file = _adapter->AdapterNewWin(cStatusBarHeight, cFileWidth, cTextScreenHeight, cCommandWidth);
	_win_for_row = _adapter->AdapterNewWin(cStatusBarHeight, cRowWidth, cTextScreenHeight, cFileWidth + cCommandWidth);
	_win_for_mode = _adapter->AdapterNewWin(cStatusBarHeight, cModeWidth, cTextScreenHeight, cRowWidth + cFileWidth + cCommandWidth);

	_adapter->AdapterInitPair(1, COLOR_BLACK, COLOR_WHITE);
	_adapter->AdapterWbkgd(_win_for_text, COLOR_PAIR(1) | A_BLINK);
	_adapter->AdapterWbkgd(_win_for_command, COLOR_PAIR(1) | A_BLINK);
	_adapter->AdapterWbkgd(_win_for_file, COLOR_PAIR(1) | A_BLINK);
	_adapter->AdapterWbkgd(_win_for_row, COLOR_PAIR(1) | A_BLINK);
	_adapter->AdapterWbkgd(_win_for_mode, COLOR_PAIR(1) | A_BLINK);

	mvwprintw(_win_for_command, 1, cCommandWidth - 1, "|");
	mvwprintw(_win_for_file, 1, cFileWidth - 1, "|");
	mvwprintw(_win_for_row, 1, cRowWidth - 1, "|");
	mvwprintw(_win_for_mode, 1, 1, "navi");

	_adapter->AdapterWrefresh(_win_for_text);
	_adapter->AdapterWrefresh(_win_for_command);
	_adapter->AdapterWrefresh(_win_for_file);
	_adapter->AdapterWrefresh(_win_for_mode);
	_adapter->AdapterWrefresh(_win_for_row);
}


void EditorView::EventWaiting()
{
	_adapter->AdapterRaw();
	_adapter->AdapterKeypad(stdscr, TRUE);
	_adapter->AdapterNoecho();

	int ch;
	while (TRUE)
	{
		ch = getch();
		_controller->TransferKey(ch);
	}
}


void EditorView::RefreshWinCommand(const MyString& string, size_t offset, const int key)
{
	_adapter->AdapterWclear(_win_for_command);
	mvwprintw(_win_for_command, 1, 1, string.c_str());
	mvwprintw(_win_for_command, 1, cCommandWidth - 1, "|");

	_y_command = cScreenHeight - 1;
	switch (key)
	{
	case(0):
		_x_command = 2;
		break;
	case(KEY_LEFT):
		if (_x_command > 2)
			_x_command--, offset--;
		_controller->TransferCommandOffset(offset);
		break;
	case(KEY_RIGHT):
		if (_x_command <= string.size())
			_x_command++, offset++;
		_controller->TransferCommandOffset(offset);
		break;
	case(KEY_BACKSPACE):
		if (string.empty())
		{
			_adapter->AdapterMove(_y, _x);
			_adapter->AdapterWrefresh(_win_for_command);
			return;
		}
		else if (_x_command > 2)
			_x_command--, offset--;
		_controller->TransferCommandOffset(offset);
		break;
	case (KEY_ENTER):
		_adapter->AdapterMove(_y, _x);
		_adapter->AdapterWrefresh(_win_for_command);
		return;
	case (ESCAPE):
		_adapter->AdapterMove(_y, _x);
		_adapter->AdapterWrefresh(_win_for_command);
		return;
	}
	_adapter->AdapterMove(_y_command, _x_command);
	_adapter->AdapterWrefresh(_win_for_command);
}

void EditorView::RefreshWinMode(Mode mode)
{
	_adapter->AdapterWclear(_win_for_mode);
	switch (mode)
	{
	case(Mode::COMMAND_MODE):
		mvwprintw(_win_for_mode, 1, 1, "command");
		break;
	case(Mode::INPUT_MODE):
		mvwprintw(_win_for_mode, 1, 1, "input");
		break;
	case(Mode::NAVI_MODE):
		mvwprintw(_win_for_mode, 1, 1, "navi");
		break;
	case(Mode::SEARCH_MODE):
		mvwprintw(_win_for_mode, 1, 1, "search");
		break;
	}
	_adapter->AdapterWrefresh(_win_for_mode);
}

void EditorView::RefreshWinHelp(const MyString& string)
{
	_adapter->AdapterWclear(_win_for_text);
	int y = 0, x = 0;
	size_t i = 0;
	while (i <= string.size())
	{
		mvwprintw(_win_for_text, y, x, "%c", string[i]);
		x++;
		if (x == cScreenWidth || string[i] == '\n')
		{
			x = 0;
			y++;
		}
		i++;
	}
	_adapter->AdapterWrefresh(_win_for_text);
}

void EditorView::RefreshWinText(const MyString& string, size_t offset, const int key)
{
	_adapter->AdapterWclear(_win_for_text);
	if (!string.empty())
	{
		switch (key)
		{
		case(KEY_RIGHT):
			NaviKeyRight(string, &offset);
			_index_in_row = offset - FindStartIndexRow(string, offset);
			break;
		case(KEY_LEFT):
			NaviKeyLeft(string, &offset);
			_index_in_row = offset - FindStartIndexRow(string, offset);
			break;
		case(KEY_DOWN):
			NaviKeyDown(string, &offset);
			break;
		case(KEY_UP):
			NaviKeyUp(string, &offset);
			break;
		case(KEY_NPAGE):
			NaviKeyPgDn(string, &offset);
			break;
		case(KEY_PPAGE):
			NaviKeyPgUp(string, &offset);
			break;
		case('^'):
			NaviStartRow(string, &offset);
			_index_in_row = offset - FindStartIndexRow(string, offset);
			break;
		case('0'):
			NaviStartRow(string, &offset);
			_index_in_row = offset - FindStartIndexRow(string, offset);
			break;
		case('$'):
			NaviEndRow(string, &offset);
			_index_in_row = offset - FindStartIndexRow(string, offset);
			break;
		case('w'):
			NaviEndWord(string, &offset);
			_index_in_row = offset - FindStartIndexRow(string, offset);
			break;
		case('b'):
			NaviStartWord(string, &offset);
			_index_in_row = offset - FindStartIndexRow(string, offset);
			break;
		case('G'):
			NaviEndFile(string, &offset);
			_index_in_row = offset - FindStartIndexRow(string, offset);
			break;
		case('g'):
			NaviStartFile(&offset);
			break;
		case ('x'):
			if (offset - 1 > 0 && string[offset] == '\n' && string[offset - 1] != '\n'
				&& string[offset + 1] != '\n')
			{
				_x--, offset--;
				CheckWidthUp();
				Scroll(string, offset);
			}
			break;
		}
		_controller->TransferFileOffset(offset);

		//print text
		int y = 0, x = 0, i = 0;
		while (y < cTextScreenHeight)
		{
			if ((_index_start_page + i) == string.size())
				break;
			mvwprintw(_win_for_text, y, x, "%c", string[_index_start_page + i]);
			x++;
			if (x == cScreenWidth || string[_index_start_page + i] == '\n')
			{
				x = 0;
				y++;
			}
			i++;
		}
		_index_end_page = _index_start_page + i - 1;
	}
	
	_adapter->AdapterWrefresh(_win_for_text);
	move(_y, _x);
}

void EditorView::RefreshWinFile(const MyString& string)
{
	_adapter->AdapterWclear(_win_for_file);
	mvwprintw(_win_for_file, 1, 1, string.c_str());
	mvwprintw(_win_for_file, 1, cFileWidth - 1, "|");
	_adapter->AdapterWrefresh(_win_for_file);
}

void EditorView::RefreshWinRow(const size_t cur_row, const size_t count_row)
{
	_adapter->AdapterWclear(_win_for_row);
	mvwprintw(_win_for_row, 1, 1, "row %ld/%ld", cur_row, count_row);
	mvwprintw(_win_for_row, 1, cRowWidth - 1, "|");
	_adapter->AdapterWrefresh(_win_for_row);
}

void EditorView::NewFile()
{
	_index_start_page = 0;
	_index_end_page = 0;
	_index_in_row = 0;
	_x = 0, _y = 0;
	_x_command = 0, _y_command = 0;
}

void EditorView::Search(const MyString& string, size_t offset, const size_t search_pos, const int key)
{
	if (key=='/')
	{
		while (search_pos > FindEndIndexRow(string, offset))
			NaviKeyDown(string, &offset);
		NaviStartRow(string, &offset);
		while (offset != search_pos)
			NaviKeyRight(string, &offset);
	}
	if (key == '?')
	{
		while (search_pos < FindStartIndexRow(string, offset))
			NaviKeyUp(string, &offset);
		NaviStartRow(string, &offset);
		while (offset != search_pos)
			NaviKeyRight(string, &offset);
	}
	_controller->TransferFileOffset(offset);
}

void EditorView::GoToRow(const MyString& string, size_t offset, const size_t row, size_t cur_row)
{
	NaviStartRow(string, &offset);
	if (cur_row < row)
		for (; cur_row < row; NaviKeyDown(string, &offset), cur_row++);
	else
		for (; cur_row > row; NaviKeyUp(string, &offset), cur_row--);
	_controller->TransferFileOffset(offset);
}


void EditorView::NaviKeyRight(const MyString& string, size_t* offset)
{
	if (string[*offset] == '\n')
		return;
	if (!string.empty() && (*offset + 1) < string.size() &&
		string[*offset + 1] != '\n')
	{
		_x++;
		(*offset)++;
		CheckWidthDown();
		Scroll(string, *offset);
	}
}

void EditorView::NaviKeyLeft(const MyString& string, size_t* offset)
{
	if (string[*offset] == '\n')
		return;
	if (!string.empty() && *offset > 0 &&
		string[*offset - 1] != '\n')
	{
		_x--;
		(*offset)--;
		CheckWidthUp();
		//CheckWidthUp();
		Scroll(string, *offset);
	}
}

void EditorView::NaviKeyDown(const MyString& string, size_t* offset)
{
	if (FindEndIndexRow(string, *offset) < string.size())
	{
		NaviEndRow(string, offset);
		_x = 0;
		_y++;
		if (string[*offset] != '\n')
			(*offset) += 2;
		else
			(*offset)++;

		if (string[*offset] != '\n')
		{
			for (size_t temp_offset = 0; temp_offset < _index_in_row && (*offset + 1) < string.size() &&
				string[*offset + 1] != '\n'; (*offset)++, _x++, temp_offset++)
				CheckWidthDown();
			CheckWidthDown();
		}

		Scroll(string, *offset);
		_controller->TransferAddRow();
	}
}

void EditorView::NaviKeyUp(const MyString& string, size_t* offset)
{
	if (FindStartIndexRow(string, *offset) >0)
	{
		NaviStartRow(string, offset);
		(*offset)--;    //jump to the end of the previous line
		_x = 0;
		size_t temp_offset;
		if ((*offset + 1) < string.size() && (string[*offset] != '\n' ||
			string[*offset - 1] != '\n'))
		{
			temp_offset = FindStartIndexRow(string, *offset);
			if (((*offset - temp_offset) % cScreenWidth) == 0)
				_y -= (*offset - temp_offset) / cScreenWidth;
			else
				_y -= ((*offset - temp_offset) / cScreenWidth + 1);
			*offset = temp_offset;

			for (temp_offset = 0; temp_offset < _index_in_row &&
				string[*offset + 1] != '\n'; (*offset)++, _x++, temp_offset++)
				CheckWidthDown();
			CheckWidthDown();
		}
		else
			_y--;
		Scroll(string, *offset);
		_controller->TransferSubRow();
	}
}

void EditorView::NaviKeyPgDn(const MyString& string, size_t* offset)
{
	if (string.empty() || (_index_end_page + 1) >= string.size())	return;
	size_t temp_end = _index_end_page;
	_index_in_row = 0;

	while (FindEndIndexRow(string, *offset) < string.size()
		&& *offset < temp_end)
		NaviKeyDown(string, offset);
	if (string[temp_end] != '\n')
	{
		NaviKeyUp(string, offset);
		while (*offset <= temp_end && *offset < string.size())
			NaviKeyRight(string, offset);
	}
	_index_start_page = *offset;
	_y = 0, _x = 0;
}

void EditorView::NaviKeyPgUp(const MyString& string, size_t* offset)
{
	if (string.empty() || _index_start_page <= 0) return;
	_index_in_row = 0;
	size_t temp_start = _index_start_page;
	while (FindStartIndexRow(string, *offset) > 0
		&& *offset > temp_start)
		NaviKeyUp(string, offset);
	if (string[temp_start] != '\n')
	{
		NaviKeyDown(string, offset);
		while (*offset <= temp_start && *offset < string.size())
			NaviKeyRight(string, offset);
	}
	_index_start_page = *offset;
	_y = 0, _x = 0;

	int y = 0, i = 0;
	size_t temp_offset;
	while (y < cTextScreenHeight && *offset >0)
	{
		i++;
		NaviKeyUp(string, offset);
		temp_offset = FindEndIndexRow(string, *offset) - *offset;
		if ((temp_offset % cScreenWidth) == 0)
			y += temp_offset / cScreenWidth;
		else
			y += temp_offset / cScreenWidth + 1;
	}
	while (y != cTextScreenHeight && *offset > 0)
	{
		*offset += cScreenWidth;
		y--;
	}
	_index_start_page = (*offset);
}


void EditorView::NaviStartRow(const MyString& string, size_t* offset)
{
	if (!string.empty() && string[*offset] != '\n')
		while (*offset > 0 && string[*offset - 1] != '\n')
			NaviKeyLeft(string, offset);
}

void EditorView::NaviEndRow(const MyString& string, size_t* offset)
{
	if (!string.empty() && string[*offset] != '\n')
		while ((*offset + 1) < string.size() && string[*offset + 1] != '\n')
			NaviKeyRight(string, offset);
}

void EditorView::NaviEndFile(const MyString& string, size_t* offset)
{
	while (FindEndIndexRow(string, *offset) < string.size())
		NaviKeyDown(string, offset);
	NaviEndRow(string, offset);
}

void EditorView::NaviStartFile(size_t* offset)
{
	NewFile();
	*offset = 0;
	_controller->TransferRow(1);
}


size_t EditorView::FindStartIndexRow(const MyString& string, const size_t offset)
{
	int i = offset;
	for (; i > 0 && string[i - 1] != '\n'; i--);
	return i;
}

size_t EditorView::FindEndIndexRow(const MyString& string, const size_t offset)
{
	size_t i = offset;
	if (string[i] == '\n') return i + 1;
	for (; i < string.size() && string[i+1] != '\n'; i++);
	return i + 1;
}


size_t EditorView::NewStartIndexDown(const MyString& string)
{
	int temp_offset = 0;
	do {
		temp_offset++;
	} while ((_index_start_page + temp_offset) < string.size() &&
		string[_index_start_page + temp_offset - 1] != '\n' &&
		temp_offset < cScreenWidth);

	return _index_start_page + temp_offset;
}


void EditorView::CheckWidthDown()
{
	if (_x == cScreenWidth)
	{
		_x = 0;
		_y++;
	}
}

void EditorView::CheckWidthUp()
{
	if (_x < 0)
	{
		_x = cScreenWidth - 1;
		_y--;
	}
}


void EditorView::Scroll(const MyString& string, const size_t offset)
{
	while(_y >= cTextScreenHeight)
	{
		_index_start_page = NewStartIndexDown(string);
		_y--;
	}
	if (_y < 0)
	{
		_index_start_page = offset - _x;
		_y = 0;
	}
}


void EditorView::NaviEndWord(const MyString& string, size_t* offset)
{
	if ((*offset + 1) < string.size() && (string[*offset + 1] == '\n'
		|| string[*offset] == '\n'))
	{
		_index_in_row = 0;
		NaviKeyDown(string, offset);
	}
	if (string[*offset] != '\n')
		while ((*offset + 1) < string.size() && SearchSpecialChar(string[*offset + 1]))
		{
			NaviKeyRight(string, offset);
			if ((*offset + 1) < string.size() && (string[*offset + 1] == '\n'
				|| string[*offset] == '\n'))
			{
				_index_in_row = 0;
				NaviKeyDown(string, offset);
			}
		}

	for (; (*offset + 1) < string.size() && string[*offset + 1] != '\n' && 
		string[*offset] != '\n' && !SearchSpecialChar(string[*offset + 1]); )
		NaviKeyRight(string, offset);
}

void EditorView::NaviStartWord(const MyString& string, size_t* offset)
{
	if ((*offset) >0 && string[*offset - 1] == '\n')
	{
		NaviKeyUp(string, offset);
		NaviEndRow(string, offset);
		_index_in_row = *offset - FindStartIndexRow(string, *offset);
	}
	while (*offset> 0 && SearchSpecialChar(string[*offset - 1]))
	{
		NaviKeyLeft(string, offset);
		if (*offset> 0 && string[*offset - 1] == '\n')
		{
			NaviKeyUp(string, offset);
			NaviEndRow(string, offset);
			_index_in_row = *offset - FindStartIndexRow(string, *offset);
		}
	}
	for (; (*offset) > 0 && string[*offset - 1] != '\n' &&
		!SearchSpecialChar(string[*offset - 1]); )
		NaviKeyLeft(string, offset);
}


bool EditorView::SearchSpecialChar(const char symbol)
{
	MyString special_char(" ,.!?@#%^&*()/+:;");
	for (size_t i = 0; i < special_char.size(); i++)
		if (special_char[i] == symbol)
			return true;
	return false;
}