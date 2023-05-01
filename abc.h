#pragma once
#define AUXS_USE_OGG
#include "../auxs/sys_io_in.h"
#include "../auxs/sys_io_out.h"
#include "../auxs/doc_html_model.h"
#include "../auxs/doc_text_repo.h"
#include "../auxs/aux_unittest.h"

using sys::byte;
using sys::str2path;
using sys::path2str;
using pix::xy;
using pix::xywh;
using pix::xyxy;
using pix::rgba;
using pix::image;
using pix::frame;
using namespace std::literals::chrono_literals;

str bold      (str s) { return "<b>" + s + "</b>"; }
str italic    (str s) { return "<i>" + s + "</i>"; }
str big       (str s) { return "<big>" + s + "</big>"; }
str small     (str s) { return "<small>" + s + "</small>"; }
str blue      (str s) { return "<font color=#000080>" + s + "</font>"; }
str lightblue (str s) { return "<font color=#0000FF>" + s + "</font>"; }
str extracolor(str s) { return "<font color=#FF8F00>" + s + "</font>"; }
str dark      (str s) { return "<font color=#505050>" + s + "</font>"; }
str gray      (str s) { return "<font color=#808080>" + s + "</font>"; }
str green     (str s) { return "<font color=#008000>" + s + "</font>"; }
str light     (str s) { return "<font color=#A0A0A0>" + s + "</font>"; }
str purple    (str s) { return "<font color=#800080>" + s + "</font>"; }
str red       (str s) { return "<font color=#B00020>" + s + "</font>"; }
str yellow    (str s) { return "<font color=#BF360C>" + s + "</font>"; }
str monospace (str s) { return "<font face=\"monospace\">" + s + "</font>"; }
str html      (str s) { return doc::html::encoded(s); }
str linked    (str s, str link) {
    return "<a href=\"" + link +
        "\">" + s + "</a>"; }

str un_msdos (str s)
{
    s.replace_all("~!",  "?");
    s.replace_all('~',1, "/");
    s.replace_all('.',2, ":");
    return s;
}
str to_msdos (str s)
{
    s.replace_all("?", "~!");
    s.replace_all("/", "~" );
    s.replace_all(":", "..");
    return s;
}

str bold_italic (str s)
{
    str html;
    int apostrophes = 0;
    bool bold = false;
    bool ital = false;
    s += ' '; // proceed ending apostrophe
    for(char c : s)
    {
        if (c == '\'' ) apostrophes++; else
        {
            if (apostrophes >= 5 ) {
                apostrophes -= 5;
                html += bold ? "</b>" : "<b>"; bold = !bold;
                html += ital ? "</i>" : "<i>"; ital = !ital;
            }
            if( apostrophes >= 3 ) {
                apostrophes -= 3;
                html += bold ? "</b>" : "<b>"; bold = !bold;
            }
            if( apostrophes >= 2 ) {
                apostrophes -= 2;
                html += ital ? "</i>" : "<i>"; ital = !ital;
            }
            html += str('\'', apostrophes);
            apostrophes = 0;
            html += c;
        }
    }
    if (ital) html += "</i>";
    if (bold) html += "</b>";
    return html;
};

struct optional_log
{
    optional_log() = default;
    optional_log(gui::console& log) : log(&log) {}
    void operator << (str s) { if (log) *log << std::move(s); }
    void operator << (array<str> ss) { if (log) *log << std::move(ss); }
    void clear () { if (log) log->clear(); }
    private: gui::console* log = nullptr;
};



