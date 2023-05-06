#pragma once
#include "eng_abc.h"
namespace eng::list
{
    array<str> sensitive = str(
    "angry, annoyed, anxious, bewildered, concerned, "
    "defiant, dejected, depressed, disbelieving, disappointed, "
    "emphatic, enthusiastic, excited, happy, horrified, "
    "impatient, incredulous, indignant, "
    "realization, relieved, "
    "sad, upset"
    ).split_by(", ");

}

/*

## uk
Moscow ## us ### {Am.}
netsuke {Jap.}

## Case
minute {noun}
minute {adjective}
Mercedes {car brand}
Mercedes {female name}
Shar Pei [Shar Pei][Shar-Pei][shar-pei][shar pei]

Are you serious? {emphatic}

## sound
triangle {musical instrument}
mortar {weapon}


miss {Miss}
miss {target} // verb, noun
miss {verb} {1} // be late
''What did I miss?''
miss {verb} {2}
''I miss you.''

(UK): /ˈkɒnkɹiːt/, /kɵnˈkɹiːt/ 
(US): /ˌkɑnˈkɹiːt/, /ˈkɑnkɹiːt/ 

*/