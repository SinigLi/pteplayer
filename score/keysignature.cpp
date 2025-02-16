/*
  * Copyright (C) 2013 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "keysignature.h"

#include <ostream>
#include <stdexcept>
#include <string>
#include <util/enumtostring.h>
#include "generalmidi.h"

const int KeySignature::MAX_NUM_ACCIDENTALS = 7;

KeySignature::KeySignature()
    : myKeyType(Major),
      myNumAccidentals(0),
      myUsesSharps(true),
      myIsVisible(false),
      myIsCancellation(false)
{
}

KeySignature::KeySignature(KeyType type, int accidentals, bool usesSharps)
    : myKeyType(type),
      myNumAccidentals(accidentals),
      myUsesSharps(usesSharps),
      myIsVisible(false),
      myIsCancellation(false)
{
}

bool KeySignature::operator==(const KeySignature &other) const
{
    return myKeyType == other.myKeyType &&
           myNumAccidentals == other.myNumAccidentals &&
           myUsesSharps == other.myUsesSharps &&
           myIsVisible == other.myIsVisible &&
           myIsCancellation == other.myIsCancellation;
}

KeySignature::KeyType KeySignature::getKeyType() const
{
    return myKeyType;
}

void KeySignature::setKeyType(KeyType type)
{
    myKeyType = type;
}

int KeySignature::getNumAccidentals(bool includeCancel) const
{
    // Cancellations will always be C Major / A Minor, so if we are not
    // including the cancellation then there are no accidentals.
    if (isCancellation() && !includeCancel)
        return 0;

    return myNumAccidentals;
}

void KeySignature::setNumAccidentals(int accidentals)
{
    if (accidentals > MAX_NUM_ACCIDENTALS)
        throw std::out_of_range("Invalid number of accidentals");

    myNumAccidentals = accidentals;
}


int KeySignature::getOffsetKey() const
{
	if (myIsCancellation) {
		return 0;
	}
    if (myNumAccidentals == 0)
    {
        return 0;
    }
    else if (myNumAccidentals == 1)
    {
        if (myUsesSharps)
		{
			return 7;
        }
        else
        {
            return 5;
        }
	}
	else if (myNumAccidentals == 2)
	{
		if (myUsesSharps)
		{
			return 2;
		}
		else
		{
			return -2;
		}
	}
	else if (myNumAccidentals == 3)
	{
		if (myUsesSharps)
		{
			return 9;
		}
		else
		{
			return 3;
		}
	}
	else if (myNumAccidentals == 4)
	{
		if (myUsesSharps)
		{
			return 4;
		}
		else
		{
			return -4;
		}
	}
	else if (myNumAccidentals == 5)
	{
		if (myUsesSharps)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
	else if (myNumAccidentals == 6)
	{
		if (myUsesSharps)
		{
			return 6;
		}
		else
		{
			return -6;
		}
	}
	else if (myNumAccidentals == 7)
	{
		if (myUsesSharps)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	return 0;
}

bool KeySignature::usesSharps() const
{
    return myUsesSharps;
}

void KeySignature::setSharps(bool sharps)
{
    myUsesSharps = sharps;
}

bool KeySignature::isVisible() const
{
    return myIsVisible;
}

void KeySignature::setVisible(bool visible)
{
    myIsVisible = visible;
}

bool KeySignature::isCancellation() const
{
    return myIsCancellation;
}

void KeySignature::setCancellation(bool cancellation)
{
    myIsCancellation = cancellation;
}

std::ostream& operator<<(std::ostream &os, const KeySignature &key)
{
    const std::string type = (key.getKeyType() == KeySignature::Major) ?
        "Major" : "Minor";
    const std::string tonic = Midi::getKeyText(
            key.getKeyType() == KeySignature::Minor, key.usesSharps(),
            key.getNumAccidentals());
    const std::string separator = key.getNumAccidentals() == 0 ? "" : " -";

    os << tonic << " " << type << separator;

    for (int i = 0; i < key.getNumAccidentals(); ++i)
    {
        const int offset = key.usesSharps() ? 6 : 2;
        os << " " << Midi::getKeyText(false, key.usesSharps(), offset + i);
    }

    return os;
}

using KeyType = KeySignature::KeyType;

UTIL_DEFINE_ENUMTOSTRING(KeyType, {
    { KeyType::Major, "Major" },
    { KeyType::Minor, "Minor" },
})
