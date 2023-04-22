#pragma once
#include "eng_abc.h"
#include "media.h"
namespace media::video
{
    void resize (image<rgba> & img, int maxsizex, int maxsizey, bool sharp = true)
    {
        xy size = img.size;

        double k = sqrt ((double)(maxsizex*maxsizey) / (size.x*size.y));

        double kx = (double) size.x / maxsizex;
        double ky = (double) size.y / maxsizey;

        double fsizex = size.x * k;  int isizex = int (fsizex + 0.5);
        double fsizey = size.y * k;  int isizey = int (fsizey + 0.5);

        if (isizex > maxsizex or isizey > maxsizey // downscale
        or  isizex*isizey < maxsizex*maxsizey*90/100) // upscale
        {
            if (kx > ky) { size = maxsizex * size / size.x; k = 1/kx; }
            else         { size = maxsizey * size / size.y; k = 1/ky; }
        }
        else size = xy (isizex, isizey);

        if (k > 1.1) pix::resize  (img, size);
        if (k < 0.9) pix::resize  (img, size); if (sharp) {
        if (k < 0.1) pix::sharpen (img, 1.75); else
        if (k < 0.5) pix::sharpen (img, 1.50); else
        if (k < 1.0) pix::sharpen (img, 1.25);
        }
    }

    static xywh crop (xy size, str crop_params)
    {
        int l = 0; int r = 0;
        int t = 0; int b = 0;
                
        auto percent = [](str s){ return std::stod(str(s.from(1))) / 100; };

        for (str s: crop_params.split_strip_by(" "))
        {
            if (s.starts_with("l")) l = clamp<int>(size.x * percent(s) + 0.5); else
            if (s.starts_with("r")) r = clamp<int>(size.x * percent(s) + 0.5); else
            if (s.starts_with("t")) t = clamp<int>(size.y * percent(s) + 0.5); else
            if (s.starts_with("b")) b = clamp<int>(size.y * percent(s) + 0.5); else
            if (s.starts_with("q"))
            {
                int x = size.x;
                int y = size.y;
                int a = min (x, y);
                int X = x-a; x=X/2;
                int Y = y-a; y=Y/2;

                if (s == "ql" || s == "qt") return xywh(0,0,a,a);
                if (s == "qr" || s == "qb") return xywh(X,Y,a,a);
                if (s == "q"  || s == "qq") return xywh(x,y,a,a);

                return xywh(0,0, size.x,size.y);
            }
        }

        return xywh(l, t, size.x-l-r, size.y-t-b);
    }

    void crop (image<rgba>& img, str crop_params)
    {
        xywh R = xywh(img);
        xywh r = crop(R.size, crop_params);
        if (r == R) return;
        img = image<rgba>(
        img.crop(r));
    }

    expected<array<byte>> readsample (path original, path cache, str crop_params) try
    {
        using namespace std::literals::chrono_literals;
        if (std::filesystem::exists(cache) == false ||
            std::filesystem::last_write_time(original) >
            std::filesystem::last_write_time(cache) - 2h)
        {
            // cannot resize ani-GIFs yet
            if (original.extension() == ".gif") {
                std::filesystem::copy_file(original, cache,
                std::filesystem::copy_options::overwrite_existing);
            }
            else
            {
                image<rgba> img = pix::read(original).value(); 

                if (img.size.x > 2500 &&
                    img.size.y > 2500 &&
                    std::filesystem::file_size(original) > 5*1024*1024)
                {
                    logs::out << "copy huge " + original.string(); if (not
                    std::filesystem::copy_file(original, "../datae_huge"/original.filename(),
                    std::filesystem::copy_options::skip_existing))
                    logs::out << "<b>already exists</b>";

                    resize (img, 2500, 2500, false);
                    pix::write (img, original, 95);
                }

                crop (img, crop_params);
                resize (img, 720, 720);

                auto result = pix::write (img, cache, 95);
                if (!result.ok()) return result.error();
            }
        }

        return sys::in::bytes(cache);
    }
    catch (std::exception & e) {return
        aux::error("media::video::readsample:"
        "<br>  path: " + original.string() +
        "<br>  cache: " + cache.string() +
        "<br>  crop: " + crop_params +
        "<br>  " + e.what());
    }

    expected<array<byte>> data (const resource & r, str cropkind)
    {
        str crop;
        for (str option: r.options)
            if (option.starts_with(cropkind + " "))
                crop = option.from(5);

        str letter = eng::asciized(
            str(aux::unicode::array(r.id).front())).
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
