#include "main.h"
#include "rc.h"
#include <filesystem>

void main_window::on_paint(HDC hdc) 
{
    RECT client_rect;
    GetClientRect(*this, &client_rect);
    float win_width = static_cast<float>(client_rect.right - client_rect.left);
    float win_height = static_cast<float>(client_rect.bottom - client_rect.top);

    Gdiplus::Graphics graphics(hdc);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    graphics.Clear(Gdiplus::Color::LightGray);

    if (image) 
    {
        float img_width = static_cast<float>(image->GetWidth());
        float img_height = static_cast<float>(image->GetHeight());

#ifdef min
#undef min
#endif

        //Scale
        float scale = std::min(win_width / img_width, win_height / img_height);
        float scaled_width = img_width * scale;
        float scaled_height = img_height * scale;

        //Centriranje slike
        Gdiplus::RectF image_rect((win_width - scaled_width) / 2.0f,(win_height - scaled_height) / 2.0f,scaled_width,scaled_height);

        graphics.DrawImage(image.get(), image_rect);

        //Postavljanje pozicije teksta i velicina fonta
        Gdiplus::Font font(L"Segoe UI", 40, Gdiplus::FontStyleBold);
        Gdiplus::StringFormat format;
        format.SetAlignment(Gdiplus::StringAlignmentCenter);
        format.SetLineAlignment(Gdiplus::StringAlignmentFar);

        float shadow_offset = 5.0f;

        //Crtanje sjene
        Gdiplus::SolidBrush shadow_brush(Gdiplus::Color(200, 0, 0, 0));
        graphics.DrawString(fileName.c_str(), -1, &font,Gdiplus::PointF(win_width / 2.0f + shadow_offset, win_height - 20.0f + shadow_offset),&format, &shadow_brush);

        //Crtanje teksta
        Gdiplus::SolidBrush text_brush(Gdiplus::Color::White);
        graphics.DrawString(fileName.c_str(), -1, &font,Gdiplus::PointF(win_width / 2.0f, win_height - 20.0f),&format, &text_brush);
    }
}

void main_window::on_command(int id) 
{
    switch (id) 
    {
        case ID_OPEN: 
        {
            TCHAR file_path[MAX_PATH] = _T("");
            OPENFILENAME ofn = { sizeof(OPENFILENAME) };
            ofn.hwndOwner = *this;
            ofn.lpstrFile = file_path;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = _T("Supported Images\0*.jpg;*.jpeg;*.png;*.bmp;*.gif;*.tiff;*.emf\0All Files\0*.*\0");
            ofn.lpstrTitle = _T("Image");
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

            if (GetOpenFileName(&ofn)) 
            {
                image = std::make_unique<Gdiplus::Image>(file_path);
                fileName = std::filesystem::path(file_path).filename().wstring();
                InvalidateRect(*this, nullptr, TRUE);
            }
        break;
        }

        case ID_EXIT:
            DestroyWindow(*this);
        break;
    }
}

void main_window::on_destroy() 
{
	::PostQuitMessage(0);
}

int WINAPI _tWinMain(HINSTANCE instance, HINSTANCE, LPTSTR, int)
{
	gdiplus_application app;
	main_window wnd;
	wnd.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, _T("NWP 7"), (int64_t)LoadMenu(instance, MAKEINTRESOURCE(IDM_MAIN)));
	return app.run();
}
