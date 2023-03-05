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
            path dir = "../data/media";
            errors = sys::in::optional_text(dir/"x_one_errors.dat").lines();
            anomal = sys::in::optional_text(dir/"x_one_anomal.dat").lines();
            audiom = sys::in::optional_text(dir/"x_one_audiom.dat").lines();
            videom = sys::in::optional_text(dir/"x_one_videom.dat").lines();
            audiop = sys::in::optional_text(dir/"x_one_audiop.dat").lines();
            videop = sys::in::optional_text(dir/"x_one_videop.dat").lines();
            audioq = sys::in::optional_text(dir/"x_one_audioq.dat").lines();
            videoq = sys::in::optional_text(dir/"x_one_videoq.dat").lines();
        }
        void save ()
        {
            path dir = "../data/media";
            sys::out::write(dir/"x_one_errors.dat", errors);
            sys::out::write(dir/"x_one_anomal.dat", anomal);
            sys::out::write(dir/"x_one_audiom.dat", audiom);
            sys::out::write(dir/"x_one_videom.dat", videom);
            sys::out::write(dir/"x_one_audiop.dat", audiop);
            sys::out::write(dir/"x_one_videop.dat", videop);
            sys::out::write(dir/"x_one_audioq.dat", audioq);
            sys::out::write(dir/"x_one_videoq.dat", videoq);
        }
    }
}