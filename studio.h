#pragma once
#include "app.h"
namespace studio::one
{
    namespace report
    {
        array<str> errors;
        array<str> anomal;
        array<str> audiom, audiop, audioq;
        array<str> videom, videop, videoq;
        void load ()
        {
            path dir = "../data/report";
            errors = sys::in::optional_text(dir/"one_errors.dat").lines();
            anomal = sys::in::optional_text(dir/"one_anomal.dat").lines();
            audiom = sys::in::optional_text(dir/"one_audiom.dat").lines();
            videom = sys::in::optional_text(dir/"one_videom.dat").lines();
            audiop = sys::in::optional_text(dir/"one_audiop.dat").lines();
            videop = sys::in::optional_text(dir/"one_videop.dat").lines();
            audioq = sys::in::optional_text(dir/"one_audioq.dat").lines();
            videoq = sys::in::optional_text(dir/"one_videoq.dat").lines();
        }
        void save ()
        {
            path dir = "../data/report";
            sys::out::write(dir/"one_errors.dat", errors);
            sys::out::write(dir/"one_anomal.dat", anomal);
            sys::out::write(dir/"one_audiom.dat", audiom);
            sys::out::write(dir/"one_videom.dat", videom);
            sys::out::write(dir/"one_audiop.dat", audiop);
            sys::out::write(dir/"one_videop.dat", videop);
            sys::out::write(dir/"one_audioq.dat", audioq);
            sys::out::write(dir/"one_videoq.dat", videoq);
        }
    }
}