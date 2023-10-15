#pragma once
#include "content.h"
namespace content
{
    struct options
    {
        struct maskered
        {
            inline static
            uint32_t Mask = 0x0000000000000000;
            uint32_t mask = 0x0000000000000000;

            void operator |= (maskered m){ mask |= m.mask; }
            void operator &= (maskered m){ mask &= m.mask; }
            bool operator == (maskered m) const { return mask == m.mask; }
            bool operator != (maskered m) const { return mask != m.mask; }

            bool ok () const
            {
                uint32_t mask_less = (mask) & 0xFFFF;
                uint32_t Mask_less = (Mask) & 0xFFFF;
                uint32_t mask_more = (mask >> 16) & 0xFFFF;
                uint32_t Mask_more = (Mask >> 16) & 0xFFFF;
                return  (mask_more &  Mask_more) == mask_more
                   and  (mask_less & ~Mask_less) == mask_less;
            }
        };

        int age = 0;
        maskered   maskered;
        array<str> Maskered;
        array<str> internal;
        array<str> external;
        array<str> unknown;

        friend void operator >> (sys::in::pool& pool, options& x) {
            pool >> x.age;
            pool >> x.maskered.mask;
            pool >> x.external;
        }
        friend void operator << (sys::out::pool& pool, options const& x) {
            pool << x.age;
            pool << x.maskered.mask;
            pool << x.external;
        }
        
        static inline array<str> maskereds = str(
        "RARE-, TERM-, IT-, TECH-, ART-, SPORT-, MATH-, POLIT-, WAR-, HISTORY-, FOOD-, MILK, O13-, O14-, O15-, O16-, "
        "RARE+, TERM+, IT+, TECH+, ART+, SPORT+, MATH+, POLIT+, WAR+, HISTORY+, FOOD+, MEAT, O13+, O14+, O15+, O16+"
        ).split_by(", ");

        options () = default;
        options (str text)
        {
            for (str s: text.split_by("#"))
            {
                static array<str> ages = str("8+, 10+, 12+, 14+, 16+, 18+, 21+, 99+").split_by(", ");
                static array<str> internals = str("DEL, pix-, audio-, sound-").split_by(", ");
                static array<str> externals = str("1, 2, 3, 4, Ru, HEAD, POEM, SOUND, "
                "rare, formal, informal, literary, old-fashioned, "
                "-1sec, +1sec, +2sec, +3sec, +4sec, +5sec"
                ).split_by(", ");

                s.strip(" \t");
                if (s == "") continue;
                if (s == "COMP-") s = "IT-"; else
                if (s == "COMP+") s = "IT+"; else
                if (s == "PHYSMATH-") s = "MATH-"; else
                if (s == "PHYSMATH+") s = "MATH+"; else
                if (s == "POLITECO-") s = "POLIT-"; else
                if (s == "POLITECO+") s = "POLIT+"; else
                {}

                if (ages     .contains(s)) age = std::stoi(s); else
                if (internals.contains(s)) internal += s; else
                if (externals.contains(s)) external += s; else
                if (maskereds.contains(s))
                {
                    Maskered += s; // dangerous to generate them from mask
                    maskered.mask |= 1 << maskereds.first(s).offset();
                }
                else unknown += s;
            }
            std::ranges::sort(Maskered);
            std::ranges::sort(external);
            std::ranges::sort(internal);
        }

        void operator |= (options const& opt)
        {
            age = max(age, opt.age);
            maskered |= opt.maskered;
            Maskered += opt.Maskered;
            internal += opt.internal;
            external += opt.external;
        }

        bool operator == (options const& opt) const = default;
        bool operator != (options const& opt) const = default;

        str formatted () const
        {
            str s;
            for (auto& o: Maskered) s += " # " + o;
            for (auto& o: external) s += " # " + o;
            for (auto& o: internal) s += " # " + o;
            for (auto& o: unknown ) s += " # " + o;
            if (age > 0) s += " # " + std::to_string(age) + "+";
            s = s.from(3);
            return s;
        }
    };
}
