#include "tracks_pixmaps.h"
#include "track1_xpm.h"
#include "track2_xpm.h"
#include "track3_xpm.h"
#include "collision1_xpm.h"
#include "collision2_xpm.h"
#include "collision3_xpm.h"
#include "track1_preview_xpm.h"
#include "track2_preview_xpm.h"
#include "track3_preview_xpm.h"

xpm_row_t * const track_xpms[TRACK_XPM_COUNT] = {
    (xpm_row_t *) track1_xpm,
    (xpm_row_t *) track2_xpm,
    (xpm_row_t *) track3_xpm
};

xpm_row_t * const collision_xpms[TRACK_XPM_COUNT] = {
    (xpm_row_t *) collision1_xpm,
    (xpm_row_t *) collision2_xpm,
    (xpm_row_t *) collision3_xpm
};

xpm_row_t * const track_preview_xpms[TRACK_XPM_COUNT] = {
    (xpm_row_t *) track1_preview_xpm,
    (xpm_row_t *) track2_preview_xpm,
    (xpm_row_t *) track3_preview_xpm
};
