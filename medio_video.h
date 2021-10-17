#pragma once
#include "eng_abc.h"
#include "media.h"
namespace media::video
{
    void resize (image<RGBA> & img, int maxsizex, int maxsizey, bool sharp = true)
    {
        XY size = img.size;

        double k = sqrt ((double)(maxsizex*maxsizey) / (size.x*size.y));

        double kx = (double) size.x / maxsizex;
        double ky = (double) size.y / maxsizey;

        double fsizex = size.x * k;  int isizex = int (fsizex + 0.5);
        double fsizey = size.y * k;  int isizey = int (fsizey + 0.5);

        if (isizex > maxsizex || isizey > maxsizey)
        {
            if (kx > ky) { size = maxsizex * size / size.x; k = 1/kx; }
            else         { size = maxsizey * size / size.y; k = 1/ky; }
        }
        else size = XY (isizex, isizey);

        if (k < 1.0) pix::resize  (img, size); if (sharp) {
        if (k < 0.1) pix::sharpen (img, 1.75); else
        if (k < 0.5) pix::sharpen (img, 1.50); else
        if (k < 1.0) pix::sharpen (img, 1.25);
        }
    }

    void crop (image<RGBA> & img, str crop_params)
    {
        int l = 0; int r = 0;
        int t = 0; int b = 0;
                
        auto percent = [](str s){ return std::stod(str(s.from(1))) / 100; };

        for (str s : crop_params.split_by(" "))
        {
            s.strip();

            if (s.starts_with("l")) l = clamp<int>(img.size.x * percent(s) + 0.5); else
            if (s.starts_with("r")) r = clamp<int>(img.size.x * percent(s) + 0.5); else
            if (s.starts_with("t")) t = clamp<int>(img.size.y * percent(s) + 0.5); else
            if (s.starts_with("b")) b = clamp<int>(img.size.y * percent(s) + 0.5); else
            if (s.starts_with("q"))
            {
                int x = img.size.x;
                int y = img.size.y;
                int a = min (x, y);
                int X = x-a; x=X/2;
                int Y = y-a; y=Y/2;

                auto crop = img.crop();

                if (s == "ql" || s == "qt") crop = crop.crop(XYWH(0,0,a,a)); else
                if (s == "qr" || s == "qb") crop = crop.crop(XYWH(X,Y,a,a)); else
                if (s == "q"  || s == "qq") crop = crop.crop(XYWH(x,y,a,a));

                img = image<RGBA>(crop);
            }
        }

        if (l != 0 || r != 0 || t != 0 || b != 0)
            img = image<RGBA>(img.crop(XYWH(l, t,
                img.size.x-l-r, img.size.y-t-b)));
    }

    expected<array<byte>> readsample (path original, path cache, str crop_params) try
    {
        using namespace std::literals::chrono_literals;
        if (std::filesystem::exists(cache) == false ||
            std::filesystem::last_write_time(original) >
            std::filesystem::last_write_time(cache) - 2h)
        {
            if (original.extension() == ".gif") {
                // cannot resize ani-GIFs yet
                std::filesystem::copy_file(original, cache,
                std::filesystem::copy_options::overwrite_existing);
            }
            else
            {
                image<RGBA> img = pix::read(original).value(); 

                if (img.size.x > 2500 &&
                    img.size.y > 2500 &&
                    std::filesystem::file_size(original) > 5*1024*1024)
                {
                    *report::out << "copy huge " + original.string();
                    if (!std::filesystem::copy_file(original, "../datae_huge"/original.filename(),
                         std::filesystem::copy_options::skip_existing))
                        *report::out << "<b>already exists</b>";

                    resize (img, 2500, 2500, false);
                    pix::write (img, original, 95);
                }

                crop (img, crop_params);
                resize (img, 720, 720);

                auto result = pix::write (img, cache, 95);
                if (!result.ok()) return result.error();
            }
        }

        return dat::in::bytes(cache);
    }
    catch (std::exception & e) {return
        aux::error("media::video::readsample:"
        "<br>  path: " + original.string() +
        "<br>  cache: " + cache.string() +
        "<br>  crop: " + crop_params +
        "<br>  " + e.what());
    }

    inline expected<array<byte>> data (const resource & r)
    {
        str crop;
        for (str option : r.options)
            if (option.starts_with("crop "))
                crop = option.from(5);

        str letter = eng::asciized(
            str(aux::unicode::glyphs(r.id).upto(1))).
            ascii_lowercased();

        str id = r.id; if (crop != "")
        {
            std::filesystem::path fn = std::string(r.id);
            str stem = fn.stem().string();
            str ext = fn.extension().string();
            str cc = " ## crop " + crop;
            if (not stem.ends_with(cc))
                id = stem + cc + ext;
        }

        str cache = "../data/!cache/"
            + r.kind + "/"
            + letter + "/"
            + id;

        return readsample(r.path, std::string(cache), crop);
    }
}
