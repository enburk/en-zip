#pragma once
#include "eng_abc.h"
#include "media.h"
namespace media::audio
{
    static auto qq (path path){ return "\"" + str(path) + "\""; }

    static void sox (str begin, path destination, str end = "")
    {
        path exe = "c:\\ogg2mp3\\sox.exe";
        path log = "../data/!cache/log.txt";

        str
        command = begin + " " + qq(destination) + " " + end;
        command.strip();

        sys::process::options opt;
        opt.hidden = true,
        opt.ascii = true,
        opt.out = log;

        sys::process p(exe, command, opt);          
        p.wait();

        using namespace std::chrono_literals;
        for (int i=0; i<200; i++)
            if (std::filesystem::exists(destination))
                return; else std::this_thread::sleep_for(100ms);

        throw std::runtime_error("waited for "
            + command + " in vain");
    }

    static void combine
    (
        array<path> srcoggg, path dstogg,
        double delay, double pause, double close,
        bool overwrite = false)
    {
        if (srcoggg.empty()) return;

        if (!overwrite && std::filesystem::exists(dstogg)) return;

        path tmpdir = "../data/!cache/!temp";
        path srcdir = "../data/!cache/!temp/srcdir";

        if (
        std::filesystem::exists(srcdir))
        std::filesystem::remove_all(srcdir);
        std::filesystem::create_directories(srcdir);

        auto format = [](const char * fmt, auto arg)
        {
            char buff[32];
            std::sprintf(buff, fmt, arg);
            return std::string(buff);
        };

        ///  std::format("{:3.1f}", delay);
        auto sec1 = format("%3.1f", delay);
        auto sec2 = format("%3.1f", pause);
        auto sec3 = format("%3.1f", close);

        path Delay = tmpdir / (sec1 + ".wav");
        path Pause = tmpdir / (sec2 + ".wav");
        path Close = tmpdir / (sec3 + ".wav");

        auto b1 = not std::filesystem::exists(Delay);
        auto b2 = not std::filesystem::exists(Pause);
        auto b3 = not std::filesystem::exists(Close);

        if (b1) sox("-n -r 44100 -c 1", Delay, "trim 0.0 " + sec1);
        if (b2) sox("-n -r 44100 -c 1", Pause, "trim 0.0 " + sec2);
        if (b3) sox("-n -r 44100 -c 1", Close, "trim 0.0 " + sec3);

        int nn = 0;
            
        std::filesystem::copy_file(Delay, srcdir / format("%05d.wav", ++nn),
        std::filesystem::copy_options::overwrite_existing);
        
        for (path srcogg : srcoggg)
        {
            path wav = tmpdir / "wav.wav";
            path src = tmpdir / ("src" + srcogg.extension().string());
            path num = srcdir / format("%05d.wav", ++nn);
        
            std::filesystem::copy_file(srcogg, src,
            std::filesystem::copy_options::overwrite_existing);
        
            sox("--combine concatenate " + qq(src) +
                " -r 44100 -c 1", wav);
            sox("--combine concatenate " + qq(wav) + " " + qq(Pause) +
                " -r 44100 -c 1", num);
        }
        
        std::filesystem::copy_file(Close, srcdir / format("%05d.wav", ++nn),
        std::filesystem::copy_options::overwrite_existing);
        
        path dst = tmpdir / ("dst" + dstogg.extension().string());
        
        sox(" --combine concatenate " + qq(srcdir/"*.wav"), dst);
        
        std::filesystem::create_directories(dstogg.parent_path());
        std::filesystem::rename(dst, dstogg);
    }

    expected<array<byte>> readsample (path original, path cache, str crop, str fade) try
    {
        using namespace std::literals::chrono_literals;
        if (std::filesystem::exists(cache) == false ||
            std::filesystem::last_write_time(original) >
            std::filesystem::last_write_time(cache) - 2h)
        {
            std::filesystem::create_directories(cache.parent_path());

            if (original.extension() == ".ogg")
            std::filesystem::copy(original, cache,
            std::filesystem::copy_options::overwrite_existing); else
            combine(array<path>{original}, cache, 0.1, 0.0, 0.0, false);

            path temp = cache; temp.replace_extension(".ogg.ogg");

            if (crop != "")
            {
                crop.replace_all("-", " =");
                if (crop.ends_with("="))
                    crop.truncate();

                std::filesystem::rename(cache, temp);
                sox(qq(temp), cache, " trim " + crop);
                std::filesystem::remove(temp);
            }
            if (fade != "")
            {
                str in  = fade == "fade out" ? "0" : "1";
                str out = fade == "fade in"  ? "0" : "1";

                std::filesystem::rename(cache, temp);
                sox(qq(temp), cache, " fade t " + in + " 0 " + out);
                std::filesystem::remove(temp);
            }
        }

        return sys::bytes(cache);
    }
    catch (std::exception & e) { return
        aux::error("media::audio::readsample:"
        "<br>  path: " + str(original) +
        "<br>  cache: " + str(cache) +
        "<br>  " + e.what());
    }

    expected<array<byte>> data (const resource & r, str)
    {
        str crop;
        for (str option : r.options)
            if (option.starts_with("crop "))
                crop = option.from(5);

        str fade;
        for (str option : r.options)
            if (option.starts_with("fade"))
                fade = option;

        str letter = eng::asciized(
            str(aux::unicode::array(r.id).front())).
            ascii_lowercased();

        std::filesystem::path fn = std::string(r.id);
        str stem = str(fn.stem());
        str id = stem + ".ogg";

        str cache = "../data/!cache/"
            + r.kind + "/"
            + letter + "/"
            + id;

        return readsample(r.path,
            std::string(cache),
            crop, fade);
    }
}
