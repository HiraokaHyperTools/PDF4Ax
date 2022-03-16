#pragma once

#include "PaperSizeLite.h"

class PaperSizeUtil {
public:
	static bool Guess(float mmWidth, float mmHeight, PaperSizeLite& ps) {
		// cm
		float X = (std::min)(mmWidth, mmHeight) / 10;
		float Y = (std::max)(mmWidth, mmHeight) / 10;

		ps.dmOrientation = (mmWidth <= mmHeight) ? DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE;

		{ 	// B5
			float D = 2, Px = 18.2f, Py = 25.72f;
			if (Px - D <= X && X <= Px + D && Py - D <= Y && Y <= Py + D) {
				ps.dmPaperSize = DMPAPER_B5;
				return true;
			}
		}
		{	// A4
			float D = 2, Px = 21.00f, Py = 29.70f;
			if (Px - D <= X && X <= Px + D && Py - D <= Y && Y <= Py + D) {
				ps.dmPaperSize = DMPAPER_A4;
				return true;
			}
		}
		{	// B4
			float D = 2, Px = 25.72f, Py = 36.41f;
			if (Px - D <= X && X <= Px + D && Py - D <= Y && Y <= Py + D) {
				ps.dmPaperSize = DMPAPER_B4;
				return true;
			}
		}
		{ 	// A3
			float D = 2, Px = 29.7f, Py = 42.0f;
			if (Px - D <= X && X <= Px + D && Py - D <= Y && Y <= Py + D) {
				ps.dmPaperSize = DMPAPER_A3;
				return true;
			}
		}
		{ 	// B3
			float D = 2, Px = 36.41f, Py = 51.51f;
			if (Px - D <= X && X <= Px + D && Py - D <= Y && Y <= Py + D) {
				ps.Apply("B3", 1433, 2028);
				return true;
			}
		}
		{ 	// A2
			float D = 2, Px = 42.00f, Py = 59.40f;
			if (Px - D <= X && X <= Px + D && Py - D <= Y && Y <= Py + D) {
				ps.dmPaperSize = DMPAPER_A2;
				return true;
			}
		}
		{ 	// B2
			float D = 2, Px = 51.51f, Py = 72.81f;
			if (Px - D <= X && X <= Px + D && Py - D <= Y && Y <= Py + D) {
				ps.Apply("B2", 2028, 2867);
				return true;
			}
		}
		{ 	// A1
			float D = 2, Px = 59.41f, Py = 84.10f;
			if (Px - D <= X && X <= Px + D && Py - D <= Y && Y <= Py + D) {
				ps.Apply("A1", 2339, 3311);
				return true;
			}
		}
		{ 	// B1
			float D = 2, Px = 72.81f, Py = 103.01f;
			if (Px - D <= X && X <= Px + D && Py - D <= Y && Y <= Py + D) {
				ps.Apply("B1", 2867, 4056);
				return true;
			}
		}
		{ 	// A0
			float D = 2, Px = 84.10f, Py = 118.89f;
			if (Px - D <= X && X <= Px + D && Py - D <= Y && Y <= Py + D) {
				ps.Apply("A0", 3311, 4680);
				return true;
			}
		}
		{ 	// B0
			float D = 2, Px = 103.01f, Py = 145.59f;
			if (Px - D <= X && X <= Px + D && Py - D <= Y && Y <= Py + D) {
				ps.Apply("B0", 4056, 5732);
				return true;
			}
		}

		ps.dmPaperSize = DMPAPER_A4;
		return false;
	}
};
