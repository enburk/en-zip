#pragma once
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "media.h"
namespace media::audio
{
    struct decoder
    {
        std::vector<byte> output;
        int channels = 0;
        int samples = 0;
        int bps = 0;

        decoder (const array<byte> & input)
        {
            if (input.empty()) return;

            OggVorbis_File vf;
            byte pcmout[4096];
            int current_section;
            int eof=0;
            int rc;

            auto data = (char*)(input.data());
            auto size = input.size();

            rc = ov_open_callbacks(0, &vf, data, size, OV_CALLBACKS_DEFAULT);
            if (rc < 0) return; // "Input does not appear to be an Ogg bitstream."

            vorbis_info *vi=ov_info(&vf,-1);
            channels = vi->channels;
            samples = vi->rate;
            bps = 16;
          
            while (!eof)
            {
                long ret = ov_read(&vf,
                    (char*)(pcmout), sizeof(pcmout),
                    0, 2, 1, &current_section);

                if (ret == 0) eof=1; else
                if (ret <  0) {} // minor error
                else output.insert(output.end(),
                    std::begin(pcmout),
                    std::begin(pcmout)+ret);
            }

            ov_clear(&vf);
        }
    };

    expected<array<byte>> readsample (path original, path cache) try
    {
        return dat::in::bytes(original);
    }
    catch (std::exception & e) { return
        ::data::error("media::audio::readsample:"
        "<br>  path: " + original.string() +
        "<br>  cache: " + cache.string() +
        "<br>  " + e.what());
    }

    expected<array<byte>> data (const resource & r)
    {
        str letter = eng::asciized(
            str(unicode::glyphs(r.id).upto(1))).
            ascii_lowercased();

        str cache = "../data/.cache/"
            + r.kind + "/"
            + letter + "/"
            + r.id;

        return readsample(r.path, std::string(cache));
    }
}
