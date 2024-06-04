#ifndef TEST_WX_WX_H
#define TEST_WX_WX_H

#include "bitmap.h"

class wxWindow {};
class wxScrolledWindow : public wxWindow {};
class wxPanel : public wxWindow {};
class wxFrame : public wxWindow {};
class wxBoxSizer {};
class wxTextCtrl {};
class wxPaintEvent {};
class wxDC {};
class wxStaticBitmap {};
class wxStaticText {};
class wxApp {};
class wxString {};
class wxCommandEvent {};
using wxWindowID = int;

#define DECLARE_EVENT_TABLE()
#define WXUNUSED(identifier) identifier

#endif
