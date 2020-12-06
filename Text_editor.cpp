#include "Text_editor.h"


int main()
{
    EditorModel model;
    EditorController controller(&model);
    Adapter adapter;
    EditorView view(&model, &controller, &adapter);
    view.EventWaiting();
    return 0;
}