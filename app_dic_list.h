#include "app.h"
namespace app::dic::list
{
    struct list : gui::widget<list>
    {
        gui::widgetarium<gui::button> words;
        gui::property<int> current = 0;
        gui::property<int> origin = 0;
        gui::schema skin_middle;
        gui::schema skin_edge;

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height;
                int n = H / h;

                for (int i=0; i<n; i++) {
                    auto & word = i < words.size() ? words(i) : words.emplace_back();
                    word.coord = XYWH(0, h*i, W, h);
                }
                words.truncate(n);
                //on_change(&skin);
                refresh();
            }
            if (what == &skin)
            {
                //skin_edge   = gui::skins[skin.now];
                //skin_middle = gui::skins[skin.now];
                //skin_middle.normal.back_color = 
                //if (words.size() > 0) {
                //    words(0).skin = skin_first;
                //}
            }
            if (what == &current) refresh();
            if (what == &origin) refresh();
        }

        void refresh ()
        {
            origin.now = clamp<int>(origin.now, -1, eng::vocabulary.size());

            for (int i=0; i<words.size(); i++)
            {
                int n = origin.now + i;
                words(i).text.text =
                    0 <= n && n < eng::vocabulary.size() ?
                    eng::vocabulary[n] : "";
            }
        }

        void on_mouse_wheel (XY p, int delta) override
        {
            origin = origin.now + delta;
        }
    };

    struct area : gui::widget<area>
    {
        gui::area<list> list;
        gui::area<gui::text::editor> word;
        gui::canvas tool;
        gui::button up, down, page_up, page_down;

        void on_change (void* what) override
        {
            if (what == &coord && coord.was.size != coord.now.size)
            {
                int W = coord.now.w; if (W <= 0) return;
                int H = coord.now.h; if (H <= 0) return;
                int h = gui::metrics::text::height;
                int l = gui::metrics::line::width*3;
                int n = (H-2*l) / h;
                int y = 0;

                list.coord = XYWH(0, 0, W, h*(n-4) + 2*l); y += list.coord.now.size.y;
                word.coord = XYWH(0, y, W, h*2); y += word.coord.now.size.y;
                tool.coord = XYXY(0, y, W, H);
            }
            if (what == &skin)
            {
                word.object.background.color = gui::skins[skin.now].white;
                word.object.view.color = gui::skins[skin.now].black;

                tool.color = gui::skins[skin.now].light.back_color;
            }
        }
    };
}

