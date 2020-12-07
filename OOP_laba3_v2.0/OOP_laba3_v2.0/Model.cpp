#include "Text_editor.h"

void Observable::AddObserver(Observer* observer)
{
	_observers.push_back(observer);
}

void Observable::NotifyUpdateWinText(const MyString& string, size_t offset, const int key)const
{
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->RefreshWinText(string, offset, key);
}

void Observable::NotifyUpdateWinHelp(const MyString& string)const
{
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->RefreshWinHelp(string);
}

void Observable::NotifyUpdateWinCommand(const MyString& string, size_t offset, const int key)const
{
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->RefreshWinCommand(string, offset, key);
}

void Observable::NotifyUpdateWinMode(const Mode mode)const
{
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->RefreshWinMode(mode);
}

void Observable::NotifyUpdateWinRow(const size_t cur_row, const size_t count_row)const
{
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->RefreshWinRow(cur_row, count_row);
}

void Observable::NotifyUpdateWinFile(const MyString& string)const
{
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->RefreshWinFile(string);
}

void Observable::NotifyNewFile()
{
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->NewFile();
}

void Observable::NotifySearch(const MyString& string, const size_t offset, const size_t search_pos, const int key)
{
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->Search(string, offset, search_pos, key);
}

void Observable::NotifyGoToRow(const MyString& string, const size_t offset, const size_t row, const size_t cur_row)
{
	int size = _observers.size();
	for (int i = 0; i < size; i++)
		_observers[i]->GoToRow(string, offset, row, cur_row);
}


EditorModel::EditorModel()
{
	_mode = Mode::NAVI_MODE;
	_modificate = false;
	_total_row = 0;
	_cur_row = 0;
	_offset_file = 0;
	_offset_command = 0;
}


void EditorModel::EventHanding(const int key)
{
	switch (_mode)
	{
	case(Mode::COMMAND_MODE):
		ModeCommand(key);
		break;
	case(Mode::NAVI_MODE):
		ModeNavi(key);
		break;
	case(Mode::INPUT_MODE):
		ModeInput(key);
		break;
	case(Mode::SEARCH_MODE):
		ModeSearch(key);
		break;
	case(Mode::HELP):
		ModeHelp(key);
		break;
	}
}


void EditorModel::ModeNavi(const int key)
{
	if (_text_buffer == "y")
	{
		switch (key)
		{
		case('y'):
			CopyRow();
			_text_buffer.clear();
			return;
			break;
		case('w'):
			CopyWord();
			_text_buffer.clear();
			return;
			break;
		}	
	}
	else if (_text_buffer == "g")
	{
		if (key == 'g') 
		{
			NotifyUpdateWinText(_file, _offset_file, 'g');
			NotifyUpdateWinRow(_cur_row, _total_row);
			_text_buffer.clear();
			return;
		}
	}
	else if (_text_buffer == "d")
	{
		switch (key)
		{
		case('d'):
			DeleteRow();
			NotifyUpdateWinText(_file, _offset_file, 0);
			NotifyUpdateWinRow(_cur_row, _total_row);
			_text_buffer.clear();
			return;
		case('i'):
			_text_buffer.append(1, static_cast<const char>(key));
			return;
		}
	}
	else if (_text_buffer == "di")
	{
		if (key == 'w')
		{
			DeleteWord();
			NotifyUpdateWinText(_file, _offset_file, 0);
			NotifyUpdateWinRow(_cur_row, _total_row);
			_text_buffer.clear();
			return;
		}
	}
	else if (std::atoi(_text_buffer.c_str()))
	{
		if (key == 'G')
		{
			NotifyGoToRow(_file, _offset_file, std::atoi(_text_buffer.c_str()), _cur_row);
			NotifyUpdateWinRow(_cur_row, _total_row);
			NotifyUpdateWinText(_file, _offset_file, 0);
			_text_buffer.clear();
			return;
		}
		else if (key >= '0' && key <= '9')
		{
			_text_buffer.append(1, static_cast<const char>(key));
			return;
		}
	}

	_text_buffer.clear();
	switch (key)
	{
	case (':'):
		//Entering Command Mode
		_mode = Mode::COMMAND_MODE;
		_text_buffer.clear();
		_text_buffer.append(1, static_cast<const char>(key));
		_offset_command = 0;
		NotifyUpdateWinCommand(_text_buffer, _offset_command, 0);
		NotifyUpdateWinMode(_mode);
		break;
	case('/'):
		_mode = Mode::SEARCH_MODE;
		_text_buffer.clear();
		_text_buffer.append(1, static_cast<const char>(key));
		_offset_command = 0;
		NotifyUpdateWinCommand(_text_buffer, _offset_command, 0);
		NotifyUpdateWinMode(_mode);
		break;
	case('?'):
		_mode = Mode::SEARCH_MODE;
		_text_buffer.clear();
		_text_buffer.append(1, static_cast<const char>(key));
		_offset_command = 0;
		NotifyUpdateWinCommand(_text_buffer, _offset_command, 0);
		NotifyUpdateWinMode(_mode);
		break;
	case ('i'):
		_mode = Mode::INPUT_MODE;
		NotifyUpdateWinMode(_mode);
		break;
	case ('I'):
		NotifyUpdateWinText(_file, _offset_file, '0');
		_mode = Mode::INPUT_MODE;
		NotifyUpdateWinMode(_mode);
		break;
	case ('A'):
		NotifyUpdateWinText(_file, _offset_file, '$');
		_mode = Mode::INPUT_MODE;
		NotifyUpdateWinMode(_mode);
		break;
	case ('r'):
		_text_buffer.append(1, static_cast<const char>(key));
		_mode = Mode::INPUT_MODE;
		NotifyUpdateWinMode(_mode);
		break;
	case ('S'):
		while (_file[_offset_file] != '\n' && _offset_file < _file.size() + 1)
		{
			_file.erase(_offset_file, 1);
			_modificate = true;
			NotifyUpdateWinText(_file, _offset_file, 'x');
		}
		_mode = Mode::INPUT_MODE;
		NotifyUpdateWinMode(_mode);
		break;
	case(KEY_LEFT):
		NotifyUpdateWinText(_file, _offset_file, KEY_LEFT);
		break;
	case(KEY_RIGHT):
		NotifyUpdateWinText(_file, _offset_file, KEY_RIGHT);
		break;
	case(KEY_DOWN):
		NotifyUpdateWinText(_file, _offset_file, KEY_DOWN);
		break;
	case(KEY_UP):
		NotifyUpdateWinText(_file, _offset_file, KEY_UP);
		break;
	case(KEY_NPAGE):
		NotifyUpdateWinText(_file, _offset_file, KEY_NPAGE);
		break;
	case(KEY_PPAGE):
		NotifyUpdateWinText(_file, _offset_file, KEY_PPAGE);
		break;
	case ('^'):
		NotifyUpdateWinText(_file, _offset_file, '^');
		break;
	case ('0'):
		NotifyUpdateWinText(_file, _offset_file, '0');
		break;
	case ('$'):
		NotifyUpdateWinText(_file, _offset_file, '$');
		break;
	case ('w'):
		NotifyUpdateWinText(_file, _offset_file, 'w');
		break;
	case ('b'):
		NotifyUpdateWinText(_file, _offset_file, 'b');
		break;
	case ('G'):
		NotifyUpdateWinText(_file, _offset_file, 'G');
		break;
	case (KEY_HOME):
		NotifyUpdateWinText(_file, _offset_file, 'g');
		break;
	case(KEY_END):
		NotifyUpdateWinText(_file, _offset_file, 'G');
		break;
	case ('x'):
		if (_offset_file < _file.size() + 1 && _file[_offset_file] != '\n')
		{
			_file.erase(_offset_file, 1);
			_modificate = true;
			NotifyUpdateWinText(_file, _offset_file, 'x');
		}
		break;
	case ('p'):
		InsertRow();
		NotifyUpdateWinText(_file, _offset_file, 'p');
		break;
	case ('y'):
		_text_buffer.clear();
		_text_buffer.append(1, static_cast<const char>(key));
		break;
	case ('g'):
		_text_buffer.clear();
		_text_buffer.append(1, static_cast<const char>(key));
		break;
	case ('d'):
		_text_buffer.clear();
		_text_buffer.append(1, static_cast<const char>(key));
		break;
	default:
		if (key >= '0' && key <='9')
			_text_buffer.append(1, static_cast<const char>(key));
		else
			_text_buffer.clear();
		break;
	}
	NotifyUpdateWinRow(_cur_row, _total_row);
}

void EditorModel::ModeCommand(const int key)
{
	switch (key)
	{
	case (KEY_IC):
		break;
	case (KEY_NPAGE):
		break;
	case (KEY_PPAGE):
		break;
	case (KEY_DOWN):
		break;
	case (KEY_UP):
		break;
	case (KEY_LEFT):
		NotifyUpdateWinCommand(_text_buffer, _offset_command,KEY_LEFT);
		break;
	case (KEY_RIGHT):
		NotifyUpdateWinCommand(_text_buffer, _offset_command, KEY_RIGHT);
		break;
	case (KEY_BACKSPACE):
		CommandKeyBackspace();
		NotifyUpdateWinCommand(_text_buffer, _offset_command, KEY_BACKSPACE);
		break;
	case (KEY_ENTER):
		CommandKeyEnter();
		NotifyUpdateWinCommand(_text_buffer, _offset_command, KEY_ENTER);
		break;
	case (ESCAPE): //Exiting command mode
		_text_buffer.clear();
		_mode = Mode::NAVI_MODE;
		NotifyUpdateWinCommand(_text_buffer, _offset_command, ESCAPE);
		break;
	default:
		if (_offset_command < _text_buffer.size())
		{
			_text_buffer.insert(_offset_command + 1, 1, static_cast<const char>(key));
			NotifyUpdateWinCommand(_text_buffer, _offset_command, KEY_RIGHT);
		}
		break;
	}

	NotifyUpdateWinMode(_mode);
}

void EditorModel::ModeInput(const int key)
{
	switch (key)
	{
	case (KEY_HOME):
		NotifyUpdateWinText(_file, _offset_file, 'g');
		break;
	case(KEY_END):
		NotifyUpdateWinText(_file, _offset_file, 'G');
		break;
	case (ESCAPE): //Exiting command mode
		_mode = Mode::NAVI_MODE;
		break;
	case(KEY_LEFT):
		NotifyUpdateWinText(_file, _offset_file, KEY_LEFT);
		break;
	case(KEY_RIGHT):
		NotifyUpdateWinText(_file, _offset_file, KEY_RIGHT);
		break;
	case(KEY_DOWN):
		NotifyUpdateWinText(_file, _offset_file, KEY_DOWN);
		break;
	case(KEY_UP):
		NotifyUpdateWinText(_file, _offset_file, KEY_UP);
		break;
	case(KEY_NPAGE):
		NotifyUpdateWinText(_file, _offset_file, KEY_NPAGE);
		break;
	case(KEY_PPAGE):
		NotifyUpdateWinText(_file, _offset_file, KEY_PPAGE);
		break;
	case (KEY_BACKSPACE):
		InputKeyBackspace();
		break;
	case(KEY_DC):
		InputKeyDelete();
		break;
	case (KEY_ENTER):
		InputKeyEnter();
		break;
	default:
		if (_text_buffer == "r")
		{
			_text_buffer.clear();
			_file.insert(_offset_file, 1, static_cast<const char>(key));
			_file.erase(_offset_file + 1, 1);
			_modificate = true;
			_mode = Mode::NAVI_MODE;
			NotifyUpdateWinMode(_mode);
			NotifyUpdateWinText(_file, _offset_file, 0);
			return;
		}
		_text_buffer.clear();
		_file.insert(_offset_file, 1, static_cast<const char>(key));
		_modificate = true;
		NotifyUpdateWinText(_file, _offset_file, KEY_RIGHT);
		break;
	}
	NotifyUpdateWinMode(_mode);
	NotifyUpdateWinRow(_cur_row, _total_row);
}

void EditorModel::ModeHelp(const int key)
{
	if (key == ESCAPE)
	{
		_mode = Mode::NAVI_MODE;
		NotifyUpdateWinText(_file, _offset_file, 0);
		NotifyUpdateWinMode(_mode);
		NotifyUpdateWinRow(_cur_row, _total_row);
	}
}

void EditorModel::ModeSearch(const int key)
{
	if (_text_buffer.empty())
	{
		switch (key)
		{
		case(ESCAPE):
			_text_buffer.clear();
			_mode = Mode::NAVI_MODE;
			NotifyUpdateWinCommand(_text_buffer, _offset_command, ESCAPE);
			break;
		case('n'):
			_text_buffer.append(1, static_cast<const char>(key));
			SearchKeyEnter(_offset_file + 1);
			_text_buffer.clear();
			NotifyUpdateWinText(_file, _offset_file, 0);
			break;
		case('N'):
			_text_buffer.append(1, static_cast<const char>(key));
			SearchKeyEnter(_offset_file - 1);
			_text_buffer.clear();
			NotifyUpdateWinText(_file, _offset_file, 0);
			break;
		case('/'):
			_text_buffer.append(1, static_cast<const char>(key));
			_offset_command = 0;
			NotifyUpdateWinCommand(_text_buffer, _offset_command, 0);
			break;
		case('?'):
			_text_buffer.append(1, static_cast<const char>(key));
			_offset_command = 0;
			NotifyUpdateWinCommand(_text_buffer, _offset_command, 0);
			break;
		}
	}
	else
	{
		//to control the input of the search string
		switch (key)
		{
		case(ESCAPE):
			_text_buffer.clear();
			_mode = Mode::NAVI_MODE;
			NotifyUpdateWinCommand(_text_buffer, _offset_command, ESCAPE);
			break;
		case (KEY_IC):
			break;
		case (KEY_NPAGE):
			break;
		case (KEY_PPAGE):
			break;
		case (KEY_DOWN):
			break;
		case (KEY_UP):
			break;
		case (KEY_LEFT):
			NotifyUpdateWinCommand(_text_buffer, _offset_command, KEY_LEFT);
			break;
		case (KEY_RIGHT):
			NotifyUpdateWinCommand(_text_buffer, _offset_command, KEY_RIGHT);
			break;
		case (KEY_BACKSPACE):
			CommandKeyBackspace();
			NotifyUpdateWinCommand(_text_buffer, _offset_command, KEY_BACKSPACE);
			break;
		case (KEY_ENTER):
			SearchKeyEnter(_offset_file);
			_text_buffer.clear();
			NotifyUpdateWinCommand(_text_buffer, _offset_command, 0);
			NotifyUpdateWinText(_file, _offset_file, 0);
			break;
		default:
			if (_offset_command < _text_buffer.size())
			{
				_text_buffer.insert(_offset_command + 1, 1, static_cast<const char>(key));
				NotifyUpdateWinCommand(_text_buffer, _offset_command, KEY_RIGHT);
			}
			break;
		}
	}
	NotifyUpdateWinMode(_mode);
	NotifyUpdateWinRow(_cur_row, _total_row);
}


void EditorModel::CommandKeyEnter()
{
	_mode = Mode::NAVI_MODE;
	if (_text_buffer[1] == 'o' && _text_buffer[2] == ' ')
	{
		if (!OpenFile())
			_text_buffer = "File not found.";
		else
			_text_buffer.clear();
	}
	else if (_text_buffer[1] == 'w' && _text_buffer[2] == ' ')
	{
		_text_buffer.erase(0, 3);
		WriteFile(_text_buffer);
		_text_buffer.clear();
	}
	else if (_text_buffer == ":q!")
	{
		ExitEditor();
	}
	else if (_text_buffer == ":q")
	{
		if (!_modificate)
			ExitEditor();
		else
			_text_buffer = "File has been modified. Use \'q!\'.";
	}
	else if (_text_buffer == ":x")
	{
		WriteFile(_file_name);
		ExitEditor();
	}
	else if (_text_buffer == ":w")
	{
		WriteFile(_file_name);
		_text_buffer.clear();
	}
	else if (_text_buffer == ":wq!")
	{
		WriteFile(_file_name);
		ExitEditor();
	}
	else if (_text_buffer == ":h")
	{
		_mode = Mode::HELP;
		_text_buffer.clear();
		NotifyUpdateWinRow(0, 0);
		OpenHelpFile();
		NotifyUpdateWinHelp(_help);
	}
	else
	{
		_text_buffer.erase(0, 1);
		size_t res = std::atoi(_text_buffer.c_str());
		if (!res)
			_text_buffer = "Error command.";
		else
		{
			if (res <= _total_row)
			{
				NotifyGoToRow(_file, _offset_file, res, _cur_row);
				_text_buffer.clear();
				NotifyUpdateWinRow(_cur_row, _total_row);
				NotifyUpdateWinText(_file, _offset_file, 0);
			}
			else
				_text_buffer = "Incorrect data.";
		}
	}

}

void EditorModel::CommandKeyBackspace()
{
	if (_text_buffer.size() > 1 && _offset_command > 0)
		_text_buffer.erase(_offset_command, 1);
	else if (_text_buffer.size() == 1)
	{
		_text_buffer.erase(_offset_command, 1);
		_text_buffer.clear();
		_mode = Mode::NAVI_MODE;
	}
}

void EditorModel::InputKeyBackspace()
{
	if (_offset_file > 0)
	{
		if (_file[_offset_file - 1] == '\n')
		{
			NotifyUpdateWinText(_file, _offset_file, KEY_UP);
			NotifyUpdateWinText(_file, _offset_file, '$');
			_file.erase(_offset_file + 1, 1);
			_total_row--;
		}
		else
		{
			NotifyUpdateWinText(_file, _offset_file, KEY_LEFT);
			_file.erase(_offset_file + 1, 1);
		}
		NotifyUpdateWinText(_file, _offset_file, 0);
		_modificate = true;
	}
}

void EditorModel::InputKeyEnter()
{
	if (_offset_file == 0 || _file[_offset_file - 1] == '\n')
	{
		NotifyUpdateWinText(_file, _offset_file, KEY_RIGHT);
		_file.insert(_offset_file - 1, 1, '\n');
		NotifyUpdateWinText(_file, _offset_file, KEY_UP);
		NotifyUpdateWinText(_file, _offset_file, '0');

	}
	else
	{
		NotifyUpdateWinText(_file, _offset_file, KEY_LEFT);
		_file.insert(_offset_file + 1, 1, '\n');
		NotifyUpdateWinText(_file, _offset_file, KEY_DOWN);
		NotifyUpdateWinText(_file, _offset_file, '0');
	}
	_total_row++;
}

void EditorModel::InputKeyDelete()
{
	if (_offset_file <= _file.size())
	{
		_file.erase(_offset_file, 1);
		_modificate = true;
		if (_file[_offset_file] != '\n')
			NotifyUpdateWinText(_file, _offset_file, 'x');
		else
		{
			NotifyUpdateWinText(_file, _offset_file, 0);
			_total_row--;
		}
	}
}

void EditorModel::SearchKeyEnter(const size_t offset)
{
	if (_text_buffer[0] == '/' || _text_buffer[0] == '?')
	{
		_search_string.clear();
		_search_string.append(_text_buffer);
		_search_string.erase(0, 1);
	}
	if (_text_buffer[0] == '/' || _text_buffer[0] == 'n')
	{
		size_t position = _file.find(_search_string.c_str(), offset);
		if (position == -1)
			return;
		NotifySearch(_file, _offset_file, position, '/');
	}
	if (_text_buffer[0] == '?' || _text_buffer[0] == 'N')
	{
		size_t position = FindRow(_search_string, offset);
		if (position == -1)
			return;
		NotifySearch(_file, _offset_file, position, '?');
	}
}


bool EditorModel::OpenFile()
{
	std::ifstream file;

	_text_buffer.erase(0, 3);
	file.open(_text_buffer.c_str());
	if (!file.is_open())
		return false;

	_file_name.clear();
	_file_name.append(_text_buffer);
	_file.clear();

	_total_row = 0;
	_offset_file = 0;
	_cur_row = 1;

	char ch;
	for (; file;)
	{
		file.get(ch);
		if (ch == '\n')
			_total_row++;
		if (file.eof())
		{
			_total_row++;
			break;
		}
		_file.append(1, ch);
	}
	NotifyNewFile();
	NotifyUpdateWinText(_file, _offset_file, 0);
	NotifyUpdateWinFile(_file_name);
	NotifyUpdateWinRow(_cur_row, _total_row);
	file.close();
	return true;
}

void EditorModel::OpenHelpFile()
{
	_help.clear();
	std::ifstream file;
	file.open("help.txt");
	if (!file.is_open())
		return;
	char ch;
	for (; file;)
	{
		file.get(ch);
		if (file.eof())
			break;
		_help.append(1, ch);
	}
	file.close();
}

void EditorModel::ExitEditor()
{
	_text_buffer.clear();
	_file.clear();
	endwin();
	exit(0);
}

void EditorModel::WriteFile(const MyString& file_name)
{
	std::ofstream file(file_name.c_str());
	file << _file;
	file.close();
}


void EditorModel::CopyRow()
{
	if (_file.empty()) return;
	_copy_string.clear();
	size_t temp_offset_start = _offset_file, temp_offset_end = _offset_file;
	for (; temp_offset_start > 0 && _file[temp_offset_start - 1] != '\n'; temp_offset_start--);
	for (; temp_offset_end < _file.size() && _file[temp_offset_end + 1] != '\n'; temp_offset_end++);
	_copy_string.append(_file.c_str(), temp_offset_start, temp_offset_end - temp_offset_start + 1);
}

void EditorModel::CopyWord()
{
	if (_file.empty()) return;
	_copy_string.clear();
	size_t temp_offset_start = _offset_file, temp_offset_end = _offset_file;
	for (; temp_offset_start > 0 && _file[temp_offset_start - 1] != ' '
		&& _file[temp_offset_start - 1] != '\n'; temp_offset_start--);
	for (; temp_offset_end < _file.size() && _file[temp_offset_end + 1] != ' '
		&& _file[temp_offset_end + 1] != '\n'; temp_offset_end++);
	_copy_string.append(_file.c_str(), temp_offset_start, temp_offset_end - temp_offset_start + 1);
	_copy_string.append(" ");
}

void EditorModel::InsertRow()
{
	if (_file.empty() || _copy_string.empty()) return;
	_file.insert(_offset_file + 1, _copy_string.c_str());
	_modificate = true;
}

void EditorModel::DeleteRow()
{
	if (_file.empty()) return;
	if (_file.find("\n") == -1) //the only row in the file
	{
		_file.clear();
	}
	else if (_file.find("\n", _offset_file) == -1) //the last row in the file
	{
		NotifyUpdateWinText(_file, _offset_file, KEY_UP);
		NotifyUpdateWinText(_file, _offset_file, '$');
		_file.erase(_offset_file + 1, _file.size() - _offset_file + 1);	
	}
	else 
	{
		NotifyUpdateWinText(_file, _offset_file, '0');
		if (_file[_offset_file] == '\n')
		{
			_file.erase(_offset_file, 1);
			_total_row--;
			_modificate = true;
			return;
		}
		size_t temp_offset_end = _offset_file;
		for (; _offset_file > 0 && _file[_offset_file - 1] != '\n'; _offset_file--);
		for (; temp_offset_end < _file.size() && _file[temp_offset_end + 1] != '\n'; temp_offset_end++);
		_file.erase(_offset_file, temp_offset_end - _offset_file + 2);
	}
	_total_row--;
	_modificate = true;
}

void EditorModel::DeleteWord()
{
	if (_file.empty() || _file[_offset_file] == '\n') return;
	//find start and end word
	if (_offset_file > 0 && _file[_offset_file - 1] != ' '
		&& _file[_offset_file - 1] != '\n')
		NotifyUpdateWinText(_file, _offset_file, 'b');	
	size_t temp_offset_end = _offset_file;
	for (; temp_offset_end < _file.size() && _file[temp_offset_end + 1] != ' '
		&& _file[temp_offset_end + 1] != '\n'; temp_offset_end++);


	if ((_offset_file == 0 || _file[_offset_file - 1] == '\n') 	//one word per row
		&& (_file[temp_offset_end + 1] == '\n' || temp_offset_end + 1 >= _file.size())){
		DeleteRow();
	}
	else
	{
		if (_file[temp_offset_end + 1] == ' ') {
			_file.erase(_offset_file, temp_offset_end + 2 - _offset_file);
		}
		else if (_file[temp_offset_end + 1] == '\n' || temp_offset_end + 1 >= _file.size()) //last word in row
		{
			NotifyUpdateWinText(_file, _offset_file, KEY_LEFT);
			_file.erase(_offset_file+1, temp_offset_end - _offset_file);
		}
	}
	_modificate = true;
}

size_t EditorModel::FindRow(const MyString& string, const size_t index)
{
	if (index < 0 || index < string.size())
		return -1;
	size_t i = index - string.size();
	while (i > 0)
	{
		if (std::equal(string.c_str(), string.c_str() + string.size(), _file.c_str() + i))
			return i;
		i--;
	}
	return -1;
}


void EditorModel::AddNumberRow()
{
	_cur_row++;
}

void EditorModel::SubNumberRow()
{
	_cur_row--;
}

void EditorModel::NumberRow(const size_t number)
{
	_cur_row = number;
}