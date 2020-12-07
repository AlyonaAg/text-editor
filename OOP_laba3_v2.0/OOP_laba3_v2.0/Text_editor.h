#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <windows.h>
#include <string>
#include <vector>
#include "curses.h"
#include "C:\Users\Asus\source\repos\MyString\MyString\my_class.h"

#define KEY_BACKSPACE 8
#define KEY_ENTER 13
#define ESCAPE 27

class EditorController;

enum class Mode
{
	NAVI_MODE = 0,
	INPUT_MODE,
	SEARCH_MODE,
	COMMAND_MODE,
	HELP
};


class Target
{
public:
	virtual int AdapterRaw() = 0;
	virtual int AdapterNoecho() = 0;
	virtual int AdapterKeypad(WINDOW*, bool) = 0;
	virtual WINDOW* ApapterInitscr() = 0;
	virtual int ApapterStartColor() = 0;
	virtual int AdapterResizeTerm(int, int) = 0;
	virtual WINDOW* AdapterNewWin(int, int, int, int) = 0;
	virtual int AdapterInitPair(short, short, short) = 0;
	virtual int AdapterWbkgd(WINDOW*, chtype) = 0;
	virtual int AdapterWrefresh(WINDOW*) = 0;
	virtual int AdapterWclear(WINDOW*) = 0;
	virtual int AdapterMove(int, int) = 0;
	virtual int AdapterEndWin() = 0;
};

class Adapter : public Target
{
public:
	virtual int AdapterRaw() { return raw(); }
	virtual int AdapterNoecho() { return noecho(); }
	virtual int AdapterKeypad(WINDOW* window, bool value) { return keypad(window, value); }
	virtual WINDOW* ApapterInitscr() { return initscr(); }
	virtual int ApapterStartColor() { return start_color(); }
	virtual int AdapterResizeTerm(int height, int windth) { return resize_term(height, windth); };
	virtual WINDOW* AdapterNewWin(int nrows, int ncol, int _y, int _x) { return newwin(nrows, ncol, _y, _x); };
	virtual int AdapterInitPair(short pair, short f, short d) { return init_pair(pair, f, d); }
	virtual int AdapterWbkgd(WINDOW* window, chtype color) { return wbkgd(window, color); }
	virtual int AdapterWrefresh(WINDOW* window) { return wrefresh(window); }
	virtual int AdapterWclear(WINDOW* window) { return wclear(window); }
	virtual int AdapterMove(int _y, int _x) { return move(_y, _x); }
	virtual int AdapterEndWin() { return endwin(); }
};


class Observer
{
public:
	virtual void RefreshWinText(const MyString& string, size_t offset, const int key) = 0;
	virtual void RefreshWinHelp(const MyString& string) = 0;
	virtual void RefreshWinCommand(const MyString& string, size_t offset, const int key) = 0;
	virtual void RefreshWinMode(const Mode mode) = 0;
	virtual void RefreshWinRow(const size_t cur_row, const  size_t count_row) = 0;
	virtual void RefreshWinFile(const MyString& string) = 0;

	virtual void NewFile() = 0;
	virtual void Search(const MyString& string, size_t offset, size_t search_pos, const int key) = 0;
	virtual void GoToRow(const MyString& string, size_t offset, const size_t row, size_t cur_row) = 0;
};


class Observable
{
public:
	void AddObserver(Observer* observer);
	void NotifyUpdateWinText(const MyString& string, size_t offset, const int key) const;
	void NotifyUpdateWinHelp(const MyString& string) const;
	void NotifyUpdateWinCommand(const MyString& string, size_t offset, const int key) const;
	void NotifyUpdateWinMode(const Mode mode) const;
	void NotifyUpdateWinRow(const size_t cur_row, const size_t count_row) const;
	void NotifyUpdateWinFile(const MyString& string) const;

	void NotifyNewFile();
	void NotifySearch(const MyString& string, size_t offset, const size_t search_pos, const int key);
	void NotifyGoToRow(const MyString& string, size_t offset, const size_t row, const size_t cur_row);
private:
	std::vector<Observer*> _observers;
};


class EditorModel : public Observable
{
public:
	EditorModel();
	~EditorModel() {};

	void EventHanding(const int key);
	void ModeCommand(const int key);
	void ModeNavi(const int key);
	void ModeInput(const int key);
	void ModeSearch(const int key);
	void ModeHelp(const int key);

	void CommandKeyEnter();
	void CommandKeyBackspace();
	void InputKeyBackspace();
	void InputKeyEnter();
	void InputKeyDelete();
	void SearchKeyEnter(const size_t offset);

	bool OpenFile();
	void WriteFile(const MyString& file_name);
	void ExitEditor();
	void OpenHelpFile();
	void Number() {};

	void SetFileOffset(size_t offset_file) { _offset_file = offset_file; }
	void SetCommandOffset(size_t offset_command) { _offset_command = offset_command; }

	void CopyRow();
	void CopyWord();
	void InsertRow();
	void DeleteRow();
	void DeleteWord();
	size_t FindRow(const MyString& string, const size_t index);

	void AddNumberRow();
	void SubNumberRow();
	void NumberRow(const size_t number);
private:
	Mode _mode;

	MyString _text_buffer;		//for command, search, etc...
	MyString _file;				//file content
	MyString _file_name;
	MyString _copy_string;		//for command 'yy' and 'yw'
	MyString _help;				//for command ':h'
	MyString _search_string;

	bool _modificate;

	size_t _total_row;
	size_t _cur_row;

	size_t _offset_file;				//line shift (in text)
	size_t _offset_command;				//line shift (in command)
};

class EditorView : public Observer
{
public:
	EditorView(EditorModel* model, EditorController* controller, Adapter* adapter);
	~EditorView() { endwin(); };

	void EventWaiting();

	virtual void RefreshWinText(const MyString& string, size_t offset, const int key);
	virtual void RefreshWinHelp(const MyString& string);
	virtual void RefreshWinCommand(const MyString& string, size_t offset, const int key);
	virtual void RefreshWinMode(const Mode mode);
	virtual void RefreshWinRow(const size_t cur_row, const size_t count_row);
	virtual void RefreshWinFile(const MyString& string);

	virtual void Search(const MyString& string, size_t offset, size_t search_pos, int key);
	virtual void GoToRow(const MyString& string, const size_t offset, const size_t row, size_t cur_row);
	virtual void NewFile();

	void NaviKeyRight(const MyString& string, size_t* offset);
	void NaviKeyLeft(const MyString& string, size_t* offset);
	void NaviKeyDown(const MyString& string, size_t* offset);
	void NaviKeyUp(const MyString& string, size_t* offset);
	void NaviKeyPgDn(const MyString& string, size_t* offset);
	void NaviKeyPgUp(const MyString& string, size_t* offset);

	void NaviStartRow(const MyString& string, size_t* offset);
	void NaviEndRow(const MyString& string, size_t* offset);
	void NaviEndFile(const MyString& string, size_t* offset);
	void NaviStartFile(size_t* offset);
	void NaviEndWord (const MyString& string, size_t* offset);
	void NaviStartWord(const MyString& string, size_t* offset);

	size_t FindEndIndexRow(const MyString& string, const size_t offset);
	size_t FindStartIndexRow(const MyString& string, const size_t offset);

	size_t NewStartIndexDown(const MyString& string);

	void CheckWidthDown();
	void CheckWidthUp();

	void Scroll(const MyString& string, const size_t offset);

	bool SearchSpecialChar(const char symbol);
private:
	const int cScreenHeight = 30;
	const int cTextScreenHeight = 28;
	const int cStatusBarHeight = 2;
	const int cScreenWidth = 130;
	const int cCommandWidth = 50;
	const int cFileWidth = 50;
	const int cRowWidth = 20;
	const int cModeWidth = 10;

	WINDOW* _win_for_text;
	WINDOW* _win_for_command;
	WINDOW* _win_for_mode;
	WINDOW* _win_for_row;
	WINDOW* _win_for_file;

	EditorController* _controller;
	Adapter* _adapter;

	int _y, _x;
	int _y_command, _x_command;

	size_t _index_start_page;	//where does the display start from
	size_t _index_end_page;
	size_t _index_in_row;		//the maximum index in the line, on which we put the cursor when moving down or up
};

class EditorController
{
public:
	EditorController(EditorModel* model) { _model = model; };
	~EditorController() {};

	void TransferFileOffset(size_t offset);
	void TransferCommandOffset(size_t offset);
	void TransferKey(const int key);
	void TransferAddRow();
	void TransferSubRow();
	void TransferRow(const size_t number);
private:
	EditorModel* _model;
};
