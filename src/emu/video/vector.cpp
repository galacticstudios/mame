// license:BSD-3-Clause
// copyright-holders:Brad Oliver,Aaron Giles,Bernd Wiebelt,Allard van der Bas
/******************************************************************************
 *
 * vector.c
 *
 *        anti-alias code by Andrew Caldwell
 *        (still more to add)
 *
 * 040227 Fixed miny clip scaling which was breaking in mhavoc. AREK
 * 010903 added support for direct RGB modes MLR
 * 980611 use translucent vectors. Thanks to Peter Hirschberg
 *        and Neil Bradley for the inspiration. BW
 * 980307 added cleverer dirty handling. BW, ASG
 *        fixed antialias table .ac
 * 980221 rewrote anti-alias line draw routine
 *        added inline assembly multiply fuction for 8086 based machines
 *        beam diameter added to draw routine
 *        beam diameter is accurate in anti-alias line draw (Tcosin)
 *        flicker added .ac
 * 980203 moved LBO's routines for drawing into a buffer of vertices
 *        from avgdvg.c to this location. Scaling is now initialized
 *        by calling vector_init(...). BW
 * 980202 moved out of msdos.c ASG
 * 980124 added anti-alias line draw routine
 *        modified avgdvg.c and sega.c to support new line draw routine
 *        added two new tables Tinten and Tmerge (for 256 color support)
 *        added find_color routine to build above tables .ac
 *
 **************************************************************************** */

#include "emu.h"
#include "emuopts.h"
#include "rendutil.h"
#include "vector.h"

// Serial port related includes
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#ifndef _WIN32
#include <termios.h>
#endif
#include <errno.h>

#include <inttypes.h>
#include <sys/time.h>
 

#define FLT_EPSILON 1E-5

#define VECTOR_WIDTH_DENOM 512

#define MAX_POINTS 10000

#define VECTOR_SERIAL_MAX 4095

#define VECTOR_TEAM \
	"-* Vector Heads *-\n" \
	"Brad Oliver\n" \
	"Aaron Giles\n" \
	"Bernd Wiebelt\n" \
	"Allard van der Bas\n" \
	"Al Kossow (VECSIM)\n" \
	"Hedley Rainnie (VECSIM)\n" \
	"Eric Smith (VECSIM)\n" \
	"Neil Bradley (technical advice)\n" \
	"Andrew Caldwell (anti-aliasing)\n" \
	"- *** -\n"

#if 0

#define TEXTURE_LENGTH_BUCKETS      32
#define TEXTURE_INTENSITY_BUCKETS   4
#define TEXTURE_WIDTH               16

#define MAX_INTENSITY               2
#define VECTOR_BLEED                (0.25f)
#define VECTOR_INT_SCALE            (255.0f * 1.5f)


struct vector_texture
{
	render_texture *    texture;
	bitmap_argb32 *     bitmap;
};

static vector_texture *vectortex[TEXTURE_INTENSITY_BUCKETS][TEXTURE_LENGTH_BUCKETS];


static render_texture *get_vector_texture(float dx, float dy, float intensity)
{
	float length = sqrt(dx * dx + dy * dy);
	int lbucket = length * (float)TEXTURE_LENGTH_BUCKETS;
	int ibucket = (intensity / (float)MAX_INTENSITY) * (float)TEXTURE_INTENSITY_BUCKETS;
	vector_texture *tex;
	int height, x, y;
	float totalint;

	if (lbucket > TEXTURE_LENGTH_BUCKETS)
		lbucket = TEXTURE_LENGTH_BUCKETS;
	if (ibucket > TEXTURE_INTENSITY_BUCKETS)
		ibucket = TEXTURE_INTENSITY_BUCKETS;

	tex = &vectortex[ibucket][lbucket];
	if (tex->texture != NULL)
		return tex->texture;

	height = lbucket * VECTOR_WIDTH_DENOM / TEXTURE_LENGTH_BUCKETS;
	tex->bitmap = global_alloc(bitmap_argb32(TEXTURE_WIDTH, height));
	tex->bitmap.fill(rgb_t(0xff,0xff,0xff,0xff));

	totalint = 1.0f;
	for (x = TEXTURE_WIDTH / 2 - 1; x >= 0; x--)
	{
		int intensity = (int)(totalint * (1.0f - VECTOR_BLEED) * VECTOR_INT_SCALE);
		intensity = MIN(255, intensity);
		totalint -= (float)intensity * (1.0f / VECTOR_INT_SCALE);

		for (y = 0; y < height; y++)
		{
			UINT32 *pix;

			pix = (UINT32 *)bitmap.base + y * bitmap.rowpixels + x;
			*pix = rgb_t((*pix.a() * intensity) >> 8,0xff,0xff,0xff);

			pix = (UINT32 *)bitmap.base + y * bitmap.rowpixels + (TEXTURE_WIDTH - 1 - x);
			*pix = rgb_t((*pix.a() * intensity) >> 8,0xff,0xff,0xff);
		}
	}

	tex->texture = render_texture_create();
	return tex->texture;
}

#endif

#define VCLEAN  0
#define VDIRTY  1
#define VCLIP   2

// device type definition
const device_type VECTOR = &device_creator<vector_device>;

vector_device::vector_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source)
	: device_t(mconfig, type, name, tag, owner, clock, shortname, source),
		device_video_interface(mconfig, *this),
		m_vector_list(nullptr),
		m_min_intensity(255),
		m_max_intensity(0)
{
}

vector_device::vector_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, VECTOR, "VECTOR", tag, owner, clock, "vector_device", __FILE__),
		device_video_interface(mconfig, *this),
		m_vector_list(nullptr),
		m_min_intensity(255),
		m_max_intensity(0)
{
}

float vector_device::m_flicker = 0.0f;
float vector_device::m_beam_width_min = 0.0f;
float vector_device::m_beam_width_max = 0.0f;
float vector_device::m_beam_intensity_weight = 0.0f;
int vector_device::m_vector_index;

struct serial_segment_t {
	struct serial_segment_t * next;
	int intensity;
	int x0;
	int y0;
	int x1;
	int y1;

	serial_segment_t(
		int x0,
		int y0,
		int x1,
		int y1,
		int intensity
	) :
		next(NULL),
		intensity(intensity),
		x0(x0),
		y0(y0),
		x1(x1),
		y1(y1)
	{
	}
};

int
serial_open(
        const char * const dev
)
{
#ifdef _WIN32   
        const int fd = open(dev, O_RDWR);
        if (fd < 0)
            return -1;
#else    
        const int fd = open(dev, O_RDWR | O_NONBLOCK | O_NOCTTY, 0666);
        if (fd < 0)
                return -1;

        // Disable modem control signals
        struct termios attr;
        tcgetattr(fd, &attr);
        attr.c_cflag |= CLOCAL | CREAD;
        attr.c_oflag &= ~OPOST;
        tcsetattr(fd, TCSANOW, &attr);
#endif

        return fd;
}

void vector_device::serial_draw_point(
	unsigned x,
	unsigned y,
	int intensity
)
{
	// make sure that we are in range; should always be
	// due to clipping on the window, but just in case
	if (x < 0) x = 0;
	if (y < 0) y = 0;

	if (x > VECTOR_SERIAL_MAX) x = VECTOR_SERIAL_MAX;
	if (y > VECTOR_SERIAL_MAX) y = VECTOR_SERIAL_MAX;

	// always flip the Y, since the vectorscope measures
	// 0,0 at the bottom left corner, but this coord uses
	// the top left corner.
	y = VECTOR_SERIAL_MAX - y;

	unsigned bright;
	if (intensity > m_serial_bright)
		bright = 63;
	else
	if (intensity <= 0)
		bright = 0;
	else
		bright = (intensity * 64) / 256;

	if (bright > 63)
		bright = 63;

	if (m_serial_rotate == 1)
	{
		// +90
		unsigned tmp = x;
		x = VECTOR_SERIAL_MAX - y;
		y = tmp;
	} else
	if (m_serial_rotate == 2)
	{
		// +180
		x = VECTOR_SERIAL_MAX - x;
		y = VECTOR_SERIAL_MAX - y;
	} else
	if (m_serial_rotate == 3)
	{
		// -90
		unsigned t = x;
		x = y;
		y = VECTOR_SERIAL_MAX - t;
	}

	uint32_t cmd = 0
		| (2 << 30)
		| (bright & 0x3F) << 24
		| (x & 0xFFF) << 12
		| (y & 0xFFF) <<  0
		;

	//printf("%08x %8d %8d %3d\n", cmd, x, y, intensity);

	m_serial_buf[m_serial_offset++] = cmd >> 24;
	m_serial_buf[m_serial_offset++] = cmd >> 16;
	m_serial_buf[m_serial_offset++] = cmd >>  8;
	m_serial_buf[m_serial_offset++] = cmd >>  0;

	// todo: check for overflow;
	// should always have enough points
}


// This will only be called with non-zero intensity lines.
// we keep a linked list of the vectors and sort them with
// a greedy insertion sort.
void vector_device::serial_draw_line(
	float xf0,
	float yf0,
	float xf1,
	float yf1,
	int intensity
)
{
	if (m_serial_fd < 0)
		return;

	// scale and shift each of the axes.
	const int x0 = (xf0 * VECTOR_SERIAL_MAX - VECTOR_SERIAL_MAX/2) * m_serial_scale_x + m_serial_offset_x;
	const int y0 = (yf0 * VECTOR_SERIAL_MAX - VECTOR_SERIAL_MAX/2) * m_serial_scale_y + m_serial_offset_y;
	const int x1 = (xf1 * VECTOR_SERIAL_MAX - VECTOR_SERIAL_MAX/2) * m_serial_scale_x + m_serial_offset_x;
	const int y1 = (yf1 * VECTOR_SERIAL_MAX - VECTOR_SERIAL_MAX/2) * m_serial_scale_y + m_serial_offset_y;

	serial_segment_t * const new_segment
		= new serial_segment_t(x0, y0, x1, y1, intensity);

	if (this->m_serial_segments_tail)
		this->m_serial_segments_tail->next = new_segment;
	else
		this->m_serial_segments = new_segment;

	this->m_serial_segments_tail = new_segment;
}


void vector_device::serial_reset()
{
	m_serial_offset = 0;
	m_serial_buf[m_serial_offset++] = 0;
	m_serial_buf[m_serial_offset++] = 0;
	m_serial_buf[m_serial_offset++] = 0;
	m_serial_buf[m_serial_offset++] = 0;
	m_serial_buf[m_serial_offset++] = 0;
	m_serial_buf[m_serial_offset++] = 0;
	m_serial_buf[m_serial_offset++] = 0;
	m_serial_buf[m_serial_offset++] = 0;

	m_vector_transit[0] = 0;
	m_vector_transit[1] = 0;
	m_vector_transit[2] = 0;
}


void vector_device::serial_send()
{
	if (m_serial_fd < 0)
		return;

	int last_x = -1;
	int last_y = -1;

	// find the next closest point to the last one.
	// greedy sorting algorithm reduces beam transit time
	// fairly significantly. doesn't matter for the
	// vectorscope, but makes a big difference for Vectrex
	// and other slower displays.
	while(this->m_serial_segments)
	{
		int reverse = 0;
		int min = 1e6;
		serial_segment_t ** min_seg
			= &this->m_serial_segments;

		if (m_serial_sort)
		for(serial_segment_t ** s = min_seg ; *s ; s = &(*s)->next)
		{
			int dx0 = (*s)->x0 - last_x;
			int dy0 = (*s)->y0 - last_y;
			int dx1 = (*s)->x1 - last_x;
			int dy1 = (*s)->y1 - last_y;
			int d0 = sqrt(dx0*dx0 + dy0*dy0);
			int d1 = sqrt(dx1*dx1 + dy1*dy1);

			if(d0 < min)
			{
				min_seg = s;
				min = d0;
				reverse = 0;
			}

			if (d1 < min)
			{
				min_seg = s;
				min = d1;
				reverse = 1;
			}

			// if we have hit two identical points,
			// then stop the search here.
			if (min == 0)
				break;
		}

		serial_segment_t * const s = *min_seg;
		if (!s)
			break;
	
		const int x0 = reverse ? s->x1 : s->x0;
		const int y0 = reverse ? s->y1 : s->y0;
		const int x1 = reverse ? s->x0 : s->x1;
		const int y1 = reverse ? s->y0 : s->y1;

		// if this is not a continuous segment,
		// we must add a transit command
		if (last_x != x0 || last_y != y0)
		{
			serial_draw_point(x0, y0, 0);
			int dx = x0 - last_x;
			int dy = y0 - last_y;
			m_vector_transit[0] += sqrt(dx*dx + dy*dy);
		}

		// transit to the new point
		int dx = x1 - x0;
		int dy = y1 - y0;
		int dist = sqrt(dx*dx + dy*dy);

		serial_draw_point(x1, y1, s->intensity);
		last_x = x1;
		last_y = y1;

		if (s->intensity > m_serial_bright)
			m_vector_transit[2] += dist;
		else
			m_vector_transit[1] += dist;

		// delete this segment from the list
		*min_seg = s->next;
		delete s;
	}

	// ensure that we erase our tracks
	if(this->m_serial_segments != NULL)
		fprintf(stderr, "errr?\n");
	this->m_serial_segments = NULL;
	this->m_serial_segments_tail = NULL;

	// add the "done" command to the message
	m_serial_buf[m_serial_offset++] = 1;
	m_serial_buf[m_serial_offset++] = 1;
	m_serial_buf[m_serial_offset++] = 1;
	m_serial_buf[m_serial_offset++] = 1;

	size_t offset = 0;

	if(1)
	printf("%lu vectors: off=%u on=%u bright=%u%s\n",
		(unsigned long) m_serial_offset/4,
		m_vector_transit[0],
		m_vector_transit[1],
		m_vector_transit[2],
		m_serial_drop_frame ? " !" : ""
	);

	static unsigned skip_frame;
	unsigned eagain = 0;

	if (m_serial_drop_frame || skip_frame++ % 2 != 0)
	{
		// we skipped a frame, don't skip the next one
		m_serial_drop_frame = 0;
	} else
	while (offset < m_serial_offset)
	{
		size_t wlen = m_serial_offset - offset;
		if (wlen > 64)
			wlen = 64;

		ssize_t rc = write(m_serial_fd, m_serial_buf + offset, m_serial_offset - offset);
		if (rc <= 0)
		{
			eagain++;
			if (errno == EAGAIN)
				continue;
			perror(m_serial);
			close(m_serial_fd);
			m_serial_fd = -1;
			break;
		}

		offset += rc;
	}

	printf("%d eagain.\n", eagain);
	if (eagain > 20)
		m_serial_drop_frame = 1;

	serial_reset();
}



void vector_device::device_start()
{
	/* Grab the settings for this session */
	m_beam_width_min = machine().options().beam_width_min();
	m_beam_width_max = machine().options().beam_width_max();
	m_beam_intensity_weight = machine().options().beam_intensity_weight();
	m_flicker = machine().options().flicker();

	m_vector_index = 0;

	/* allocate memory for tables */
	m_vector_list = make_unique_clear<point[]>(MAX_POINTS);

	/* Setup the serial output of the XY coords if configured */
	m_serial = machine().options().vector_serial();
  	const float scale = machine().options().vector_scale();
	if (scale != 0.0)
	{
		// user specified a scale on the command line
		m_serial_scale_x = m_serial_scale_y = scale;
	} else {
		// use the per-axis scales
		m_serial_scale_x = machine().options().vector_scale_x();
		m_serial_scale_y = machine().options().vector_scale_y();
	}

	m_serial_segments = m_serial_segments_tail = NULL;

	m_serial_offset_x = machine().options().vector_offset_x();
	m_serial_offset_y = machine().options().vector_offset_y();
	m_serial_rotate = machine().options().vector_rotate();
	m_serial_bright = machine().options().vector_bright();
	m_serial_drop_frame = 0;
	m_serial_sort = 1;

	// allocate enough buffer space, although we should never use this much
	m_serial_buf = auto_alloc_array_clear(machine(), unsigned char, (MAX_POINTS+2) * 4);
	if (!m_serial_buf)
	{
		// todo: how to signal an error?
	}

	serial_reset();

	if (!m_serial || strcmp(m_serial,"") == 0)
	{
		fprintf(stderr, "no serial vector display configured\n");
		m_serial_fd = -1;
	} else {
		m_serial_fd = serial_open(m_serial);
		fprintf(stderr, "serial dev='%s' fd=%d\n", m_serial, m_serial_fd);
	}
}

void vector_device::set_flicker(float newval)
{
	m_flicker = newval;
}

float vector_device::get_flicker()
{
	return m_flicker;
}

void vector_device::set_beam_width_min(float newval)
{
	m_beam_width_min = newval;
}

float vector_device::get_beam_width_min()
{
	return m_beam_width_min;
}

void vector_device::set_beam_width_max(float newval)
{
	m_beam_width_max = newval;
}

float vector_device::get_beam_width_max()
{
	return m_beam_width_max;
}

void vector_device::set_beam_intensity_weight(float newval)
{
	m_beam_intensity_weight = newval;
}

float vector_device::get_beam_intensity_weight()
{
	return m_beam_intensity_weight;
}


/*
 * www.dinodini.wordpress.com/2010/04/05/normalized-tunable-sigmoid-functions/
 */
float vector_device::normalized_sigmoid(float n, float k)
{
	// valid for n and k in range of -1.0 and 1.0
	return (n - n * k) / (k - fabs(n) * 2.0f * k + 1.0f);
}


/*
 * Adds a line end point to the vertices list. The vector processor emulation
 * needs to call this.
 */
void vector_device::add_point(int x, int y, rgb_t color, int intensity)
{
	point *newpoint;

//printf("%d %d: %d,%d,%d @ %d\n", x, y, color.r(), color.b(), color.g(), intensity);

	// hack for the vectrex
	// -- convert "128,128,128" @ 255 to "255,255,255" @ 127
	if (color.r() == 128
	&&  color.b() == 128
	&&  color.g() == 128
	&&  intensity == 255)
	{
		color = rgb_t(255,255,255);
		intensity = 128;
	}

	intensity = MAX(0, MIN(255, intensity));

	m_min_intensity = intensity > 0 ? MIN(m_min_intensity, intensity) : m_min_intensity;
	m_max_intensity = intensity > 0 ? MAX(m_max_intensity, intensity) : m_max_intensity;

	if (m_flicker && (intensity > 0))
	{
		float random = (float)(machine().rand() & 255) / 255.0f; // random value between 0.0 and 1.0

		intensity -= (int)(intensity * random * m_flicker);

		intensity = MAX(0, MIN(255, intensity));
	}

	newpoint = &m_vector_list[m_vector_index];
	newpoint->x = x;
	newpoint->y = y;
	newpoint->col = color;
	newpoint->intensity = intensity;
	newpoint->status = VDIRTY; /* mark identical lines as clean later */

	m_vector_index++;
	if (m_vector_index >= MAX_POINTS)
	{
		m_vector_index--;
		logerror("*** Warning! Vector list overflow!\n");
	}
}


/*
 * Add new clipping info to the list
 */
void vector_device::add_clip(int x1, int yy1, int x2, int y2)
{
	point *newpoint;

	newpoint = &m_vector_list[m_vector_index];
	newpoint->x = x1;
	newpoint->y = yy1;
	newpoint->arg1 = x2;
	newpoint->arg2 = y2;
	newpoint->status = VCLIP;

	m_vector_index++;
	if (m_vector_index >= MAX_POINTS)
	{
		m_vector_index--;
		logerror("*** Warning! Vector list overflow!\n");
	}
}


/*
 * The vector CPU creates a new display list. We save the old display list,
 * but only once per refresh.
 */
void vector_device::clear_list(void)
{
	m_vector_index = 0;
}


UINT32 vector_device::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	UINT32 flags = PRIMFLAG_ANTIALIAS(screen.machine().options().antialias() ? 1 : 0) | PRIMFLAG_BLENDMODE(BLENDMODE_ADD) | PRIMFLAG_VECTOR(1);
	const rectangle &visarea = screen.visible_area();
	float xscale = 1.0f / (65536 * visarea.width());
	float yscale = 1.0f / (65536 * visarea.height());
	float xoffs = (float)visarea.min_x;
	float yoffs = (float)visarea.min_y;
	float xratio = xscale / yscale;
	float yratio = yscale / xscale;
	xratio = (xratio < 1.0f) ? xratio : 1.0f;
	yratio = (yratio < 1.0f) ? yratio : 1.0f;

	point *curpoint;
	render_bounds clip;
	int lastx = 0;
	int lasty = 0;

	curpoint = m_vector_list.get();

	screen.container().empty();
	screen.container().add_rect(0.0f, 0.0f, 1.0f, 1.0f, rgb_t(0xff,0x00,0x00,0x00), PRIMFLAG_BLENDMODE(BLENDMODE_ALPHA) | PRIMFLAG_VECTORBUF(1));

	clip.x0 = clip.y0 = 0.0f;
	clip.x1 = clip.y1 = 1.0f;

	for (int i = 0; i < m_vector_index; i++)
	{
		render_bounds coords;

		if (curpoint->status == VCLIP)
		{
			coords.x0 = ((float)curpoint->x - xoffs) * xscale;
			coords.y0 = ((float)curpoint->y - yoffs) * yscale;
			coords.x1 = ((float)curpoint->arg1 - xoffs) * xscale;
			coords.y1 = ((float)curpoint->arg2 - yoffs) * yscale;

			clip.x0 = (coords.x0 > 0.0f) ? coords.x0 : 0.0f;
			clip.y0 = (coords.y0 > 0.0f) ? coords.y0 : 0.0f;
			clip.x1 = (coords.x1 < 1.0f) ? coords.x1 : 1.0f;
			clip.y1 = (coords.y1 < 1.0f) ? coords.y1 : 1.0f;
		}
		else
		{
			float beam_intensity_width = m_beam_width_min;

			float intensity = (float)curpoint->intensity / 255.0f;

			// check for dynamic intensity
			if (m_min_intensity != m_max_intensity)
			{
				float intensity_weight = normalized_sigmoid(intensity, m_beam_intensity_weight);
				beam_intensity_width = (m_beam_width_max - m_beam_width_min) * intensity_weight + m_beam_width_min;
			}

			float beam_width = beam_intensity_width * (1.0f / (float)VECTOR_WIDTH_DENOM);

			coords.x0 = ((float)lastx - xoffs) * xscale;
			coords.y0 = ((float)lasty - yoffs) * yscale;
			coords.x1 = ((float)curpoint->x - xoffs) * xscale;
			coords.y1 = ((float)curpoint->y - yoffs) * yscale;

			// extend zero-length vector line (vector point) by quarter beam_width on both sides
			if (fabs(coords.x0 - coords.x1) < FLT_EPSILON &&
				fabs(coords.y0 - coords.y1) < FLT_EPSILON)
			{
				coords.x0 += xratio * beam_width * 0.25f;
				coords.y0 += yratio * beam_width * 0.25f;
				coords.x1 -= xratio * beam_width * 0.25f;
				coords.y1 -= yratio * beam_width * 0.25f;
			}

			if (curpoint->intensity != 0 && !render_clip_line(&coords, &clip))
			{
				screen.container().add_line(
					coords.x0, coords.y0, coords.x1, coords.y1,
					beam_width,
					(curpoint->intensity << 24) | (curpoint->col & 0xffffff),
					flags);

				serial_draw_line(
					coords.x0, coords.y0,
					coords.x1, coords.y1,
					curpoint->intensity);
			}

			lastx = curpoint->x;
			lasty = curpoint->y;
		}

		curpoint++;
	}

	serial_send();

	return 0;
}
