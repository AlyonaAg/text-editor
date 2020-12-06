#include "Text_editor.h"

void EditorController::TransferFileOffset(size_t offset)
{
	_model->SetFileOffset(offset);
}

void EditorController::TransferKey(const int key)
{
	_model->EventHanding(key);
}

void EditorController::TransferCommandOffset(size_t offset)
{
	_model->SetCommandOffset(offset);
}

void EditorController::TransferAddRow()
{
	_model->AddNumberRow();
}

void EditorController::TransferSubRow()
{
	_model->SubNumberRow();
}

void EditorController::TransferRow(const size_t number)
{
	_model->NumberRow(number);
}